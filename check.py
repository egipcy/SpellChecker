import subprocess

class color:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    YELLOW = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

p = subprocess.Popen(['./memory.sh', './TextMiningCompiler words.txt mydict.bin'],
                     stdout=subprocess.PIPE, stderr=subprocess.PIPE)

res = p.communicate()

memory = float(res[1][:-1]) / 1024

print(('TextMiningCompiler consommation max: '
        + (color.OKGREEN if memory < 512 else color.FAIL)
        + '%.0f MB' + color.ENDC) % (memory))
print('\n')



words = ['test', 'toto']
min_req = [3000, 300, 30]

nb_fails = 0
nb_null = 0
nb_ok = 0
nb_omg = 0

for distance in range(1):
    for word in words:
        b = False
        tot_time = 0
        ref_tot_time = 0
        nb_loop = 0
        while tot_time < 1:
            nb_loop += 1
            request = 'approx ' + str(distance) + ' ' + word

            p = subprocess.Popen([
                '/bin/sh', '-c', 'time -f "%e" ./TextMiningApp mydict.bin'],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            res = p.communicate(request)
            tot_time += float(res[1][:-1])
            answer = res[0]

            ref_p = subprocess.Popen([
                '/bin/sh', '-c', 'time -f "%e" ./ref/refTextMiningApp ref/refdict.bin'],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, stdin=subprocess.PIPE)
            ref_res = ref_p.communicate(request)
            ref_tot_time += float(ref_res[1].split()[-1])
            ref_answer = ref_res[0]

            if answer != ref_answer:
                b = True
                break

        nb_req = int(nb_loop / tot_time)
        ref_nb_req = int(nb_loop / ref_tot_time)

        print((color.FAIL if b else color.OKGREEN) + request + color.ENDC)
        if b:
            nb_fails += 1
            print(color.OKBLUE + ref_answer + color.ENDC)
        else:
            if nb_req < min_req[min(distance, 2)]:
                nb_null += 1
                print(color.BOLD + color.FAIL + str(nb_req) + 'r/s' + color.ENDC + ' < '
                    + color.HEADER + str(min_req[min(distance, 2)]) + 'r/s' + color.ENDC + ' < '
                    + color.OKBLUE + str(ref_nb_req) + 'r/s' + color.ENDC)
            elif nb_req > ref_nb_req:
                nb_ok += 1
                print(color.HEADER + str(min_req[min(distance, 2)]) + 'r/s' + color.ENDC + ' < '
                    + color.OKBLUE + str(ref_nb_req) + 'r/s' + color.ENDC + ' < '
                    + color.BOLD + color.YELLOW + color.UNDERLINE + str(nb_req) + 'r/s' + color.ENDC)
            else:
                nb_omg += 1
                print(color.HEADER + str(min_req[min(distance, 2)]) + 'r/s' + color.ENDC + ' < '
                    + color.BOLD + color.OKGREEN + str(nb_req) + 'r/s' + color.ENDC + ' < '
                    + color.OKBLUE + str(ref_nb_req) + 'r/s' + color.ENDC)

        print('\n')

nb_tot = nb_fails + nb_null + nb_ok + nb_omg
print(((color.OKGREEN if memory < 512 else color.FAIL) + '%.0f MB\n' + color.ENDC
        + color.FAIL + 'Fails:\t%d%%\n' + color.HEADER + 'Null:\t%d%%\n'
        + color.OKGREEN + 'OK:\t%d%%\n' + color.YELLOW + 'OMG:\t%d%%' + color.ENDC)
        % (memory, nb_fails / nb_tot * 100, nb_null / nb_tot * 100, nb_ok / nb_tot * 100, nb_omg / nb_tot * 100))
