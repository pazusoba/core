from com.android.monkeyrunner import MonkeyDevice

newimage = MonkeyDevice.takeSnapshot()
newimage.writeToFile(".", "png")