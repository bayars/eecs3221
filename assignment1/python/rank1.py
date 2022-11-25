import string

s="txxmubonuhlaryeuujgftedrmmhmaadxrplneqpwhsketqicdpqlecluydmgykrubgmpwfqviabkjoiqdftbbwwgiuudmgrdbkrr"


def superReducedString(s):
    prm = list(s)
    alphabet = list(string.ascii_lowercase)
    for i in alphabet:
        t = prm.count(i)
        if t % 2 ==0:
            while t > 0:
                prm.remove(i)
                t = prm.count(i)
        else:
            while t > 1:
                prm.remove(i)
                t = prm.count(i)
    return "Empty String" if not prm else "".join([str(i) for i in prm])


if superReducedString(s) == "tmubonuhlaryejgftedrhmdxrplneqpwhsketqicdpqlecluydmgykrubgmpwfqviabkjoiqdftgidmgrdbk":
    print("True")
else:
    print("False\n")

print(superReducedString(s))