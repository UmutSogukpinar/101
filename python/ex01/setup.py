from setuptools import find_packages, setup


setup(
    name="snapshot",
    version="0.1.0",
    description="Simple system snapshot monitoring tool",
    author="Umut",
    packages=find_packages(),
    install_requires=[
        "psutil",
    ],
    entry_points={
        "console_scripts": [
            "snapshot=snapshot.snapshot:main",
        ],
    },
)
