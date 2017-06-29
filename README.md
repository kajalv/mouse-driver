# Device Driver Implementation

This is an implementation of a custom Linux device driver. It reads mouse clicks and changes the brightness of the screen. A left click decreases brightness and a right click increases brightness.

The module has been tested on Ubuntu 14.04 with a USB mouse.

## Working

Devices are generally represented by their respective files in the */dev* directory.

Device files are created using the ```mknod``` system call.

```
mknod path type major minor
```

- *Path* - Path where the file is to be created.
- *Type* - 'c' or 'b' to indicate whether the device is a character device or a block device.
- *Major, Minor* - The major and minor number of the device. Major number identifies the device driver. Minor number is used to identify the specific instance of the device (if there is more than one). ```ls – l``` is used to find these numbers.

Device drivers can be built either as part of the kernel or separately as loadable modules. ```lsmod``` (or, alternatively, ```cat /proc/modules```) prints the contents of the ```/proc/modules``` file, which shows the loadable kernel modules are currently loaded.

Modules can be loaded using the ```insmod``` command, by giving the name of the object file (*.ko*) to be loaded.

```
insmod module_name
```
Modules can be unloaded using the ```rmmod``` command.

```
rmmod module_name
```

## Procedure

Execute once:

```
mknod /dev/mbdriver c 45 1
chmod a+r+w /dev/mbdriver
```

Repeat to test the driver: (```dmesg``` is for debugging)
```
make
insmod dev_driver.ko
./a.out
dmesg
```

Finally:
```
rmmod dev_driver
```
