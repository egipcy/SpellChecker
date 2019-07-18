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

response = raw_input('Test memory (y/N): ')
print('\n')

if response in ['y', 'Y']:
    p = subprocess.Popen(['./memory.sh', './TextMiningCompiler words.txt mydict.bin'],
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    res = p.communicate()

    memory = float(res[1][:-1]) / 1024

    print(('TextMiningCompiler consommation max: '
            + (color.OKGREEN if memory < 512 else color.FAIL)
            + '%.0f MB' + color.ENDC) % (memory))
    print('\n')



derive_stream = False
ref_nbrps = [212089.1, 54522.6, 186.0]
min_req = [3000, 300, 30]
nbreq_per_dist = [200000, 20000, 2000]

nb_fails = 0
nb_null = 0
nb_ok = 0
nb_omg = 0

for distance in range(3):
    b = False
    tot_time = 0.
    ref_tot_time = 0.
    nb_loop = 0
    while tot_time < 1:
        nb_loop += nbreq_per_dist[distance]

        p = subprocess.Popen([
            '/bin/sh', '-c', 'shuf words.txt | head -' + str(nbreq_per_dist[distance]) + ' | cut -f1 | \
            while read word; do echo approx 0 $word; done > /tmp/input.txt \
            && /usr/bin/time ./TextMiningApp ./mydict.bin < /tmp/input.txt' + (' > /dev/null ' if derive_stream else '') + ' \
            && printf "{{{" \
            && /usr/bin/time ./ref/refTextMiningApp ./ref/refdict.bin < /tmp/input.txt' + (' > /dev/null' if derive_stream else '')],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        res = p.communicate()
        assert(p.returncode == 0)

        res1 = res[1].split()
        time = -1
        ref_time = -1
        for e in res1:
            if e[-4:] == 'user':
                if time == -1:
                    time = float(e[:-4])
                else:
                    ref_time = float(e[:-4])
                    break

        tot_time += time
        ref_tot_time += ref_time

        if not derive_stream:
            answer, ref_answer = res[0].split('{{{')

            if answer != ref_answer:
                b = True
                break

    nb_req = nb_loop / tot_time
    ref_nb_req = nb_loop / ref_tot_time

    print((color.FAIL if b else color.OKGREEN) + 'Distance ' + str(distance) + color.ENDC)
    if b:
        nb_fails += 1
    else:
        if nb_req < min_req[min(distance, 2)]:
            nb_null += 1
            print(('Real: ' + color.BOLD + color.FAIL + '%dr/s' + color.ENDC + ' < '
                + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC) % (nb_req, min_req[min(distance, 2)], ref_nb_req))
            print(('Norm: ' + color.BOLD + color.FAIL + '%dr/s' + color.ENDC + ' < '
                + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC) % (nb_req * ref_nbrps[distance] / ref_nb_req, min_req[min(distance, 2)], ref_nbrps[distance]))
        elif nb_req > ref_nb_req:
            nb_omg += 1
            print(('Real: ' + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC + ' < '
                + color.BOLD + color.YELLOW + color.UNDERLINE + '%dr/s' + color.ENDC) % (min_req[min(distance, 2)], ref_nb_req, nb_req))
            print(('Norm: ' + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC + ' < '
                + color.BOLD + color.YELLOW + color.UNDERLINE + '%dr/s' + color.ENDC) % (min_req[min(distance, 2)], ref_nbrps[distance], nb_req * ref_nbrps[distance] / ref_nb_req))
        else:
            nb_ok += 1
            print(('Real: ' + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.BOLD + color.OKGREEN + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC) % (min_req[min(distance, 2)], nb_req, ref_nb_req))
            print(('Norm: ' + color.HEADER + '%dr/s' + color.ENDC + ' < '
                + color.BOLD + color.OKGREEN + '%dr/s' + color.ENDC + ' < '
                + color.OKBLUE + '%dr/s' + color.ENDC) % (min_req[min(distance, 2)], nb_req * ref_nbrps[distance] / ref_nb_req, ref_nbrps[distance]))

    print('\n')

nb_tot = nb_fails + nb_null + nb_ok + nb_omg

if response in ['y', 'Y']:
    print(((color.OKGREEN if memory < 512 else color.FAIL) + '%.0f MB\n' + color.ENDC) % (memory))

print((color.FAIL + 'Fails:\t%d%%\n' + color.HEADER + 'Null:\t%d%%\n'
        + color.OKGREEN + 'OK:\t%d%%\n' + color.YELLOW + 'OMG:\t%d%%' + color.ENDC)
        % (nb_fails / nb_tot * 100, nb_null / nb_tot * 100, nb_ok / nb_tot * 100, nb_omg / nb_tot * 100))


