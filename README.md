# macbrew
A project to use a m86k based Macs to help with homebrewing

## Virtual Serial Ports

To test locally you can set up a virtual serial port pair with:

```
socat -d -d pty,raw,echo=0 pty,raw,echo=0 &
```

Use `/dev/pts/1` as the target serial port for macbrew-proxy and 
you can use the other side of the pair to push data through. E.g `cat > /dev/pts/2`

## License

[Hippocratic License 2.1](https://firstdonoharm.dev)
