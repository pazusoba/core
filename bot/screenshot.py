from ppadb.client import Client as AdbClient

# this is the default
client = AdbClient(host="127.0.0.1", port=5037)
# use the first device, make sure there is only one device connected
device = client.devices()[0]
# grab the screenshot and save it
# result = device.screencap()
# with open("screen.png", "wb") as fp:
#     fp.write(result)

# try some simple movement
device.shell("input swipe 100 1534 100 1834 500 & input swipe 100 1834 950 1838 1000")
