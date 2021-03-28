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
