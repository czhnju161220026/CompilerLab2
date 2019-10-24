import re
import os
def extract(oldS):
    oldS = oldS[: oldS.find(":")]
    return re.sub('[^a-zA-Z0-9]', '', oldS)

def compare(output, correctOutput):
    dict_o = {}
    dict_c = {}
    for x in output:
        x = extract(x)
        if x != "":
            dict_o[x] = 1
    for x in correctOutput:
        x = extract(x)
        if x != "":
            dict_c[x] = 1
    if (len(dict_o) == 0 and len(dict_c) == 0):
        return True
    elif (len(dict_c) == 0 and len(dict_o) != 0):
        return False
    else:
        for k in dict_c.keys():
            if not k in dict_o:
                return False
        return True

def generateSingleCase(id, time):
    if time == 0:
        return "single-errid-%d.0" % (id)
    else:
        return "single-errid-%d-times-%d.0" % (id, time)

def checkCase(type):
    for i in range(10):
        cmmFile = generateSingleCase(type, i)+".cmm"
        cmmLog = generateSingleCase(type, i) + ".log"
        os.system("./Code/parser Advanced/%s > temp.output" % (cmmFile))
        myOutput = open("temp.output", "r").readlines()
        log = open("Advanced/%s" % (cmmLog), "r").readlines()
        if compare(myOutput, log):
            print("\033[32m %s passed \033[0m" % (cmmFile))
        else:
            print("\033[31m %s wrong \033[0m" % (cmmFile))

if __name__=='__main__':
    check_list = [6, 7, 8, 9, 10, 11, 12, 14,20, 21, 22, 23,    24, 25]
    for c in check_list:
        checkCase(c)
