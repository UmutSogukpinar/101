from jinja2 import Environment, FileSystemLoader
import yaml


def main():
    with open("data.yml", "r", encoding="utf-8") as file:
        data = yaml.safe_load(file)

    env = Environment(
        loader=FileSystemLoader("."),
        trim_blocks=True,
        lstrip_blocks=True
    )

    template = env.get_template("vhosts.j2")
    output = template.render(vhosts=data["vhosts"])

    with open("vhosts.conf", "w", encoding="utf-8") as file:
        file.write(output)


if __name__ == "__main__":
    main()
