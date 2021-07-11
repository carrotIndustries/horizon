import xml.etree.ElementTree as ET
import argparse
import uuid
import os
import json
import sqlite3

argp = argparse.ArgumentParser(description = "use this script with the XML files from the db/mcu directory in an STM32CubeMX installation")
argp.add_argument("source", metavar="FILE", help="Path to part XML in an STM32CubeMX installation (usually like ~/STM32CUbeMX/db/mcu/STM32****.xml)")
argp.add_argument("-P", "--pool", metavar="PATH", help="Path to destination Horizon pool", default=os.getcwd())
argp.add_argument("-f", "--package", metavar="NAME", help="Package name to use for part (example: QFP-64)")
argp.add_argument("-n", "--name", metavar="PART", help="Entity/Part name (will be used 'ref name' by default)")
argp.add_argument("-s", "--separate-power", action="store_true", help="Separate power lines from logic (one extra unit/gate)")

args = argp.parse_args()

def genid():
	return str(uuid.uuid4())

def load_json(*path):
	with open(os.path.join(*path), "r") as fd:
		return json.load(fd)

def save_json(data, *path):
	with open(os.path.join(*path), "w") as fd:
		json.dump(data, fd, sort_keys=True, indent=4, separators=(',', ': '))

pool_dir = args.pool
pool_db = sqlite3.connect(os.path.join(pool_dir, "pool.db"))

def get_package_info(name):
	c = pool_db.cursor()
	c.execute("SELECT uuid, filename FROM packages WHERE name LIKE ?", (name+'%',))
	p = c.fetchone()
	if p == None:
		raise Exception("Unable to find package: %s" % name)
	package_id = p[0]
	filename = p[1]
	package_data = load_json(pool_dir, filename)
	models = package_data.get("models", {})
	model_id = None
	for uuid, _ in models.items():
		model_id = uuid
		break
	pads = package_data.get("pads", {})
	pads_by_name = {data["name"]: uuid for uuid, data in pads.items()}
	return (package_id, model_id, pads_by_name)

ns = {'st': 'http://mcd.rou.st.com/modules.php?name=mcu'}
manufacturer = "ST"
datasheet_url = "https://st.com/"

type_dir_map = {
	"Power": "power_input",
	"Reset": "input"
}

separate_power = args.separate_power

tree = ET.parse(args.source)
root = tree.getroot()

ref_name = args.name if args.name != None else root.get("RefName")
package_name = args.package if args.package != None else root.get("Package")

MPN = ref_name
MPNs = []

(package_id, model_id, pads_by_name) = get_package_info(package_name)

tags = ["ic", "mcu"]
if ref_name.startswith("STM32"):
	tags += ["arm", "stm32"]
if ref_name.startswith("STM8"):
	tags += ["stm8"]

units = [
	{
		"name": ref_name + (" I/O" if separate_power else ""),
		"type": "unit",
		"uuid": genid(),
		"manufacturer": manufacturer,
		"pins": {},
	}
]

gate_ids = [genid()]

gates = {
	gate_ids[0]: {
		"name": "Main",
		"suffix": "",
		"swap_group": 0,
		"unit": units[0]["uuid"],
	}
}

if separate_power:
	units += [
		{
			"name": ref_name + " PWR",
			"type": "unit",
			"uuid": genid(),
			"manufacturer": manufacturer,
			"pins": {},
		}
	]

	gate_ids += [genid()]
	gates[gate_ids[1]] = {
		"name": "Power",
		"suffix": "",
		"swap_group": 0,
		"unit": units[1]["uuid"],
	}

pad_map = {}

for pin in root.findall("st:Pin", ns):
	pin_type = pin.get("Type")
	unit_no = 1 if separate_power and pin_type == "Power" else 0

	pin_id = genid()
	units[unit_no]["pins"][pin_id] = {
		"primary_name": pin.get("Name"),
		"names": [alt.get("Name")
				  for alt in pin.findall("st:Signal", ns)
				  if alt.get("Name") != "GPIO"],
		"direction": type_dir_map.get(pin_type, "bidirectional"),
		"swap_group": 0,
	}

	pad_id = pads_by_name[pin.get("Position")]
	pad_map[pad_id] = {
		"gate": gate_ids[unit_no],
		"pin": pin_id,
	}

entity = {
	"name": ref_name,
	"type": "entity",
	"uuid": genid(),
	"prefix": "U",
	"manufacturer": manufacturer,
	"tags": tags,
    "gates": gates,
}

part = {
	"type": "part",
	"uuid": genid(),
	"tags": tags,
	"value": [False, ""],
	"MPN": [False, MPN],
	"datasheet": [False, datasheet_url],
	"description": [False, ""],
	"entity": entity["uuid"],
	"inherit_model": False,
    "inherit_tags": False,
    "manufacturer": [False, manufacturer],
	"orderable_MPNs": {genid(): MPN for MPN in MPNs},
	"package": package_id,
	"pad_map": pad_map,
	"parametric": {},
}

if model_id != None:
	part["model"] = model_id

filename = ref_name + ".json"

for unit in units:
	sfx = ""
	if separate_power:
		sfx = "_io" if unit["name"].endswith("I/O") else "_pwr"
	save_json(unit, pool_dir, "units", "ic", "mcu", "stm", ref_name + sfx + ".json")
save_json(entity, pool_dir, "entities", "ic", "mcu", "stm", filename)
save_json(part, pool_dir, "parts", "ic", "mcu", "stm", filename)
