import os
from glob import glob
from setuptools import find_packages, setup

package_name = 'rover'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),

        (os.path.join('share', package_name, 'launch'), glob(os.path.join('bringup','launch','*launch.py'))), 
        (os.path.join('share', package_name, 'config'), glob(os.path.join('bringup', 'config', '*'))),
        (os.path.join('share', package_name, 'urdf'), glob(os.path.join('description', 'urdf', '*'))),
        (os.path.join('share', package_name, 'worlds'), glob(os.path.join('description', 'worlds', '*'))),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='root',
    maintainer_email='jpsalis@outlook.com',
    description='TODO: Package description',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
