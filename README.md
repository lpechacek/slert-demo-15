# slert-demo-15
SUSECon 2015 - SLERT demo

Prepare the host
----------------
* switch off HT in BIOS
* install RT kernel
* install osciloscope
  zypper ar -f http://download.suse.de/ibs/home:/LPechacek:/SLE12/SLE12-SP1/ lpechacek-sle12
  zypper in http://download.suse.de/ibs/SUSE:/SLE-12:/GA/standard/x86_64/python-matplotlib-1.3.1-1.219.x86_64.rpm \
	  http://download.suse.de/ibs/SUSE:/SLE-12:/GA/standard/noarch/python-dateutil-2.1-10.7.noarch.rpm \
	  oscilloscope
  ignore the missing python-ethtool dependency for tuna
* cpupower -c all frequency-set -g performance
* select two physically separate CPUs to run message processor and network receiver processes
* cset shield --kthread=on --cpu=X-Y

Test the hardware
-----------------
* modprobe hwlat_detector enabled=1 threshold=10
  to check hardware with latency detector, see results in
  /sys/kernel/debug/hwlat_detector/count and
  /sys/kernel/debug/hwlat_detector/sample
* optionally also check with cyclictest
* see https://www.suse.com/documentation/slerte_11/sle-rte-hardware-testing/data/sle-rte-hardware-testing.html
  for more details

Run the test
------------
* ./run_processor.sh X Y | tee /dev/shm/test-results
  Ctrl-C to stop after the test
* for i in $(seq 1 20); do ~/slert-demo-15/network-sender -n 50 <the_host>; sleep .5; done
  on another machine to send 20 batches of 50 messages to the demo system
* oscilloscope -nm 50 < /dev/shm/test-results
  to replay the results

Running the tests with simulated contention
-------------------------------------------
* supply -c option to run_processor.sh; it runs noise-maker on the same CPU as
  message-processor and instructs message-processor to perform additional
  operations on a file shared with noise-maker as part of the processing
