with open('selections_history.txt', 'r') as shread:
    with open('sh.txt', 'w+') as shwrite:
        for line in shread:
            if "Length: 0" or "Length: 1" not in line:
                shwrite.write(line)
