# nautilus-follow-symlink / caja-follow-symlink

[debian-repository]: https://packages.outlyer.net/

_nautilus-follow-symlink_ is a nautilus (GNOME Desktop) extension.

_caja-follow-symlink_ is a caja (MATE Desktop) extension.

They adds a menu entry on symbolic links to directories which open the pointed directory (the real path).

They do so both when right clicking on the folder icon or on the contents of an opened symbolic link.

![Visualisation](https://p.outlyer.net/_media/nautilus-follow-symlink/screenshots-montage.png)

Homepage: https://p.outlyer.net/nautilus-follow-symlink

## Installation

I provide both `deb` and `rpm` packages to download and install manually, e.g.:

```shell
dpkg -i nautilus-follow-symlink_1.2.0-pon.1_amd64.deb
```

For deb-based distributions you may add my [repository][debian-repository] to your sources:

```shell
echo 'deb http://packages.outlyer.net/debian stable main contrib' | sudo tee /etc/apt/sources.list.d/packages.outlyer.net.list
sudo wget http://packages.outlyer.net/public_key.gpg -O /etc/apt/trusted.gpg.d/packages.outlyer.net.gpg
sudo apt update
```

## Building from source

Download the tarball and follow the traditional `configure` + `make` workflow, e.g.:

```shell
tar xvf nautilus-follow-symlink-1.2.0.tar.gz
cd nautilus-follow-symlink-1.2.0
./configure
make
sudo make install
```

## Building from the git repository

Building from the raw source in the repository requires the GNU `auto*` set of tools.

```shell
git clone https://git.outlyer.net/outlyer/nautilus-follow-symlink/
cd nautilus-follow-symlink/nautilus
./bootstrap.sh
./configure
make
sudo make install
```