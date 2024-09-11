# Images

Classic Mac system images are stored in here.

The images are based off
the awesome System images found at
https://blog.rekawek.eu/2016/12/29/macos-images-for-iomega-zip/
with some extra development tools layered on top
which I used to get my Mac Plus up and running.

They can be used in an emulator like [Basilisk II](https://basilisk.cebix.net/)
or copied to a Zip Disk or a SD Card if you have a [SCSI2SD](http://www.codesrc.com/mediawiki/index.php/SCSI2SD)

I own a Mac Plus, which is what I'm technically targetting for this project.
However, Basilisk II doesn't support Macs prior to the IIci.
The emulators I've looked at that support the Mac Plus (like [Mini vMac](https://www.gryphel.com/c/minivmac/hardware.html#serial)) don't support serial emulation so I've gone with a IIci ROM/System 7 image and I'm just hoping that the Mac Plus doesn't act too differently.

I do a lot of my development on the Mac Plus anyway,
it's just when something breaks down (like the Zip Drive right now)
I can still do some development.
It would also be cool to try and wangle
an automatic integration test of some sort.

The IIci rom is from https://www.macintoshrepository.org/7038-all-macintosh-roms-68k-ppc-

## Installing Basilisk II

### Fedora

It is in the [RPMFusion](https://rpmfusion.org/Configuration) repo under the name "BasiliskII" (case sensitive).

```bash
$ sudo dnf install BasiliskII
```

### Debian

The VSCode dev images are based on Debian. I can see the package in the Debian repos but I can't seem to install it through apt, so I just manually install the dependencies and then fetch the deb file and install it.

```
$ apt-get install libatk1.0-0 libgtk2.0-0 libtinyxml2-6a libsdl1.2debian
$ curl -o basilisk2_0.9.20180101-1+b1_amd64.deb http://http.us.debian.org/debian/pool/contrib/b/basilisk2/basilisk2_0.9.20180101-1+b1_amd64.deb
$ dpkg -i basilisk2_0.9.20180101-1+b1_amd64.deb
```

## Descriptions

### system710-macbrew-dev.image

An image with everything needed to compile and run the `macbrew-ui` Think C project.

Before shutting down the image,
make sure that all the windows are closed
and the desktop icons haven't been moved
to facilitate automated testing.

## Running

Make sure the image has the latest code:

```
$ ./build-code-image.sh -f
```

Start the fake serial loopback devices:

```
$ socat -d -d pty,raw,echo=0 pty,raw,echo=0
```

Start macbrew-proxy:

```
$ cargo run /dev/pts/0
```

Start the emulator:

```
$ ./start.sh
```

## Troubleshooting

### Emualtor freezes while debugging during serial read/write

Not sure if this is a bug with Think C or Basilisk but it seems like the debugger freezes during serial IO/
The "solution" is to not use the debugger.
It doesn't seem to be as much of an issue on the real Mac

### SCSI2SD Notes

- I was getting protocol errors writing config. It would get to 3% and stop working. It seemed to be because the scsi2sd-util version from the MacOS app store is out of date. I got a new version from http://www.codesrc.com/files/scsi2sd/latest/mac/ and it started working.
- http://www.savagetaylor.com/2018/01/05/setting-up-your-vintage-classic-68k-macintosh-using-a-scsi2sd-adapter/ is a wealth of knowledge about setting up scsi2sd
- Since the image is also from a zip disk you might need to set the following scsi2sd settings:
  - Vendor: IOMEGA
  - Product ID: ZIP 100
  - Revision: D.09
