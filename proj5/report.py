import socket
import sys
import re

def main():
    kill = False
    if ''.join(sys.argv).find('k') > 0:
        kill = True

    host = '127.0.0.1'
    port = 5666
    addr = (host, port)
    backlog = 10 # Num of connections
    size = 4096 # Maximum data stream size
    xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(addr)
    if kill:
        s.send(xml_head + "<req>perfect numbers</req>\n<kill>True</kill>\n")
    else:
        s.send(xml_head + "<req>perfect numbers</req>\n")
    xml = s.recv(size)

    perfect_nums = []
    match = ""
    if len(xml.split("\n")) > 1:
        _re_tag_contents = "<.*>(.*)</.*>"
        for x in xml.split("\n")[1:]:
            try:
            # no idea why im getting errorsa nonetype on re.match
                match = re.match(_re_tag_contents, x).groups()[0]
                if match:
                    perfect_nums.append(match)
            except:
                if match and match not in perfect_nums:
                    perfect_nums.append(match)

    print "Perfect Numbers: "
    for pn in perfect_nums:
        print pn

if __name__ == "__main__":
    main()
