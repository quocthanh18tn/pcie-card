# Release PCIE CARD VTH 15-05-2020
### HDF và bitstream
+ HDF dùng cho bring up pcie card VHT.
+ bitstream thực hiện uplink SISO và downlink SISO.
### BINARY
+ Nạp bitstream từ Linux.
+ Cấu hình IPcore bên dưới PL từ linux thông qua SSH.
+ Các pcietool dùng để check device I2C, set config LMK....
### References
+ http://10.61.248.4:....
### Author
+ thanhpq7: bring up các feature cơ bản của pcie card VHT.
+ quyetpt: develop các feature nâng cao của pcie card VHT.