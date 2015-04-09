DHCP sniff
==========

About
-----

Detects DHCP request on the network and runs chosen command with client MAC address as last argument. This is intentionally very simple program, so it can safely run with root priviledges.

Use case
--------

You want to run some home automation script on your server, when your smartphone connects to WiFi. Just run DHCP sniff with your script as an argument. The last argument for your script will be MAC address (so you can check whether it's actually your smartphone).

Warning
-------

There's no authentication! All devices connected to your network must be trusted or your script must be harmless!

License
-------

Author: Martin Habovštiak (2015)
The code is released under the terms of MIT license.
