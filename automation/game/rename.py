import os
from glob import iglob

# replace .PNG to .png on Linux
root_dir = "**"
files = [f for f in iglob(root_dir, recursive=True) if f.endswith(".PNG")]
for f in files:
    os.rename(f, f.replace(".PNG", ".png"))
