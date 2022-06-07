#
# NOTE: This script requires the reload pluging, install it as follows:
#         vagrant plugin install vagrant-reload
#

Vagrant.configure("2") do |config|

  config.vm.provider "virtualbox" do |v|
    v.name = "xdp-skeleton"
    v.cpus = 6
    v.memory = 16384
  end

  config.vm.box = "ubuntu/jammy64"
  config.vm.hostname = "xdp-skeleton"

  config.vm.synced_folder ".", "/git"
  config.vm.provision :shell, privileged: false, path: "bootstrap.sh"

end
