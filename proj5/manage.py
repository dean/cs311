import socket
import sys
import re
import select
import Queue

perfect_nums = []
current_number = 1
top_number = 1000000

def main():
    host = '127.0.0.1'
    port = 5666
    addr = (host, port)
    backlog = 10 # Num of connections
    size = 4096 # Maximum data stream size


    # Create a TCP/IP socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setblocking(0)

    # Bind the socket to the port
    server_address = addr
    print >>sys.stderr, 'starting up on %s port %s' % server_address
    server.bind(server_address)

    # Listen for incoming connections
    server.listen(5)

    # Sockets from which we expect to read
    inputs = [ server ]

    # Sockets to which we expect to write
    outputs = [ ]

    # Outgoing message queues (socket:Queue)
    message_queues = {}

    while inputs:

        # Wait for at least one of the sockets to be ready for processing
        readable, writable, exceptional = select.select(inputs, outputs, inputs)

        # Handle inputs
        for s in readable:

            if s is server:
                # A "readable" server socket is ready to accept a connection
                connection, client_address = s.accept()
                print >>sys.stderr, 'new connection from', client_address
                connection.setblocking(0)
                inputs.append(connection)

                # Give the connection a queue for data we want to send
                message_queues[connection] = Queue.Queue()

            else:
                data = s.recv(size)
                if data:
                    print "recieved req"
                    print data
                    resp = manage(data)
                    # No more need to calc after a None resp
                    print "resp=" + str(resp)
                    if resp:
                        print "sending resp..."
                        s.send(resp)
                    else:
                        print "closing client..."


def manage(data):
    global perfect_nums
    xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    _re_tag_contents = "<.*>(.*)</.*>"
    print "data=" +data
    print data.find("<req>")
    if data.find("<req>") > 0:
        print "in req"
        # First line is xml header info
        xml = data.split("\n")[1]
        matches = re.match(_re_tag_contents, xml).groups()
        if matches:
            req = matches[0]
            if req == "range":
                print perfect_nums
                return get_number_range()
            if req == "perfect numbers":
                return get_perfect_numbers()
    elif data.find("<perfect>") > 0:
        print "in perfect area..."
        if len(data.split("\n")) > 1:
            _re_tag_contents = "<.*>(.*)</.*>"
            for x in data.split("\n")[1:]:
                try:
                # no idea why im getting errorsa nonetype on re.match
                    print "x= " + str(x)
                    match = re.match(_re_tag_contents, x).groups()[0]
                    perfect_nums.append(int(match))
                    print perfect_nums
                    print "perfect nums appending " + str(match)
                except:
                    print "perfect nums appending " + str(match)
                    if int(match) not in perfect_nums:
                        perfect_nums.append(int(match))
                    continue
    print perfect_nums
    return None


def is_xml(data):
    xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
    return data.split("\n")[0] == xml_head

def get_number_range():
    global current_number
    global top_number
    xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    start = current_number
    if top_number - (current_number + 100000) > 0:
        current_number = current_number + 100000
    elif top_number != current_number:
        current_number = top_number
    else:
        return None
    xml = (xml_head + "<start>" + str(start) + "</start>\n" +
            "<end>" + str(current_number) + "</end>\n")
    return xml

def get_perfect_numbers():
    global perfect_nums
    xml_head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    xml = ''.join(["<perfect>" + str(n) + "</perfect>\n" for n
            in perfect_nums])
    print xml
    return xml_head + xml

if __name__ == "__main__":
    main()
