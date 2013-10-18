import getopt
import os
import sys

def main():

    n = """
        73167176531330624919225119674426574742355349194934
        96983520312774506326239578318016984801869478851843
        85861560789112949495459501737958331952853208805511
        12540698747158523863050715693290963295227443043557
        66896648950445244523161731856403098711121722383113
        62229893423380308135336276614282806444486645238749
        30358907296290491560440772390713810515859307960866
        70172427121883998797908792274921901699720888093776
        65727333001053367881220235421809751254540594752243
        52584907711670556013604839586446706324415722155397
        53697817977846174064955149290862569321978468622482
        83972241375657056057490261407972968652414535100474
        82166370484403199890008895243450658541227588666881
        16427171479924442928230863465674813919123162824586
        17866458359124566529476545682848912883142607690042
        24219022671055626321111109370544217506941658960408
        07198403850962455444362981230987879927244284909188
        84580156166097919133875499200524063689912560717606
        05886116467109405077541002256983155200055935729725
        71636269561882670428252483600823257530420752963450
        """

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hp:t:c:", ["help", "problem", "term", "course"])
    except getopt.GetoptError:
        usage()
        sys.exit(0)

    problem = 0
    term = None
    course = None
    for o, a in opts:
        if o in ["-h", "--help"]:
            usage()
            sys.exit(0)
        elif o in ["-p", "--problem"]:
            problem = int(a)
        elif o in ["-t", "--term"]:
            term = a
        elif o in ["-c", "--course"]:
            course = a
        else:
            usage()
            sys.exit(0)

    if problem == 1:
        make_dirs(term, course)
        print "Directories created and sym linked!"
    elif problem == 2:
        print ("The highest product of 5 consecutive digits in the 1000 digit number" +
               " is: " + str(highest_product(n.replace("\n", "").replace(" ", ""))))
    elif problem == 3:
        sorted_names = sort_names()
        total_score = sum([score_word(sorted_names[index], index=index+1)
            for index in xrange(len(sorted_names))])
        print "Total Score for all names: " + str(total_score)
    elif problem == 4:
        print ("The number of triangle words in \"words.txt\" is: " +
                str(count_triangle_words()))
    else:
        print "The option you supplied doesn't exist!"

def usage():
    print """
Usage:
    assignment1.py -p [problem #] [--term TERM] [--course COURSE]"
Problem numbers:"
    [1] Make directories for a given term and course,"
        as well as symbolic linking website and handin folders."
    [2] Find the greatest product of 5 consecutive digits in a"
        given number that is saved in a file of your choice."
    [3] Sort a list of names from \"names.txt\" and find the name"
        score of all names combined."
    [4] Find the number of triangle words in \"words.txt\"."
"""


def make_dirs(term, course):
    path = term
    if not os.path.exists(path):
        os.mkdir(path)
    path += "/" + course

    if not os.path.exists(path):
        os.mkdir(path)

    path += "/"

    for folder in ['assignments', 'examples', 'exams',
                    'lecture_notes', 'submissions']:
        if not os.path.exists(path + folder):
            os.mkdir(path + folder)

    source = "/usr/local/classes/eecs/" + term + "/" + course + "/"
    os.symlink(source + "public_html", path + "website")
    os.symlink(source + "handin", path + "handin")

def highest_product(n):
    return max([reduce(lambda x, y: x*y, map(int, n[x:x+5])) for x in xrange(len(n)-4)])

def sort_names():
    f = open("names.txt")
    names = f.read().replace("\"", "").split(",")
    return sorted(names, key=lambda x: x)

def score_word(word, index=1):
    # Value to subtract from the ascii value of a
    # char to score a letter.
    base_ascii_val = ord('A')-1
    return sum([ord(letter) - base_ascii_val for letter in list(word)]) * index

def count_triangle_words():
    f = open("words.txt")
    words = f.read().replace("\"", "").split(",")
    scores = [score_word(word) for word in words]
    n = get_lowest_n_value(max(scores))
    triangle_numbers = get_triangle_numbers(n)
    return sum([1 if score in triangle_numbers else 0 for score in scores])

def get_triangle_numbers(n):
    # We want to go right up to n
    return [(i * (i + 1))/2 for i in xrange(n + 1)]

def get_lowest_n_value(max_value):
    i = 0
    while (i * (i + 1)) / 2 <= max_value:
        i += 1
    return i

if __name__ == "__main__":
    main()
