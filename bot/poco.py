# import unity poco driver from this path
from poco.drivers.unity3d import UnityPoco

# then initialize the poco instance in the following way
poco = UnityPoco()

# now you can play with poco
ui = poco('...')
ui.click()