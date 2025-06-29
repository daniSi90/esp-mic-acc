import matplotlib.pyplot as plt

data = [
1924, 1988, 2017, 2037, 2067, 2080, 2101, 2102, 2099, 2084, 2044, 2011, 1982, 1930, 1874, 1825, 1759, 1706, 1626, 1541, 1432, 1341, 1221, 1079, 992, 850, 717, 651, 505, 404, 323, 192, 114, 13, -73, -131, -204, -232, -255, -277, -257, -239, -201, -150, -96, -32, 31, 107, 183, 263, 340, 425, 528, 617, 717, 813, 894, 1004, 1081, 1151, 1265, 1320, 1420, 1512, 1563, 1671, 1741, 1781, 1877, 1908, 1942, 2010, 2017, 2052, 2083, 2083, 2110, 2100, 2079, 2066, 2034, 1991, 1956, 1907, 1861, 1800, 1731, 1670, 1576, 1478, 1380, 1265, 1149, 1014, 907, 804, 665, 572, 473, 346, 
]

plt.figure(figsize=(12, 5))
plt.plot(data, marker='o', linestyle='-', color='blue')
plt.title('Amplitude vs Sample Index')
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.grid(True)
plt.tight_layout()
plt.show()
