One of these files must be placed in `/var/local/natinst/bitfiles/` on the myRIO target. Choose the correct file based on your myRIO version (1900 or 1950).

```shell
cd misc
sftp admin@172.22.11.2
...
cd /var/local/natinst/bitfiles/
put NiFpga_MyRio1900Fpga60.lvbitx
chmod 777 NiFpga_MyRio1900Fpga60.lvbitx
```