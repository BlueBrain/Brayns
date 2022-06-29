from sysconfig import get_path, sys

full_path = get_path('platlib')
prefix = sys.prefix + '/'
relative_path = full_path.replace(prefix, '')
print(relative_path)
