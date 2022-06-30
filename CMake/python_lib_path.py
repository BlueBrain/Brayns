from sysconfig import get_path, sys
from pathlib import Path

full_path = Path(get_path('platlib'))
prefix = Path(sys.prefix)
relative_path = full_path.relative_to(prefix)
print(relative_path)
