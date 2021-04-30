#!/usr/bin/python


# https://xlsxwriter.readthedocs.io/example_chart_column.html

import sys
import re
import fileinput
import xlsxwriter
import csv

# log file globals
frames = []
freqs = [] # tuples
def parse_log(file_name):
    log_fh = open(file_name, "r")
    length_regexp = 'FA*B*A*B*(\d+)sinfreq = (\d+)\.(\d+)'
    re_length = re.compile(length_regexp)
    prev_frame = 0;
    #for line in fileinput.input():
    for line in log_fh:
        #print line
        matches = re_length.search(line)
        if matches:
            freq = matches.group(2) + "." + matches.group(3)
            frame = matches.group(1)
            frames.append(matches.group(1))
            tup = prev_frame, frame, freq
            freqs.append(tup)
            print "Fr: " + str(prev_frame) + ":" + frame + " = Freq " + freq
            prev_frame = frame

# csv globals
fr = []
rpos = []
rcmd = []
ppos = []
pcmd = []
def parse_csv(file_name):
    csv_fh = open(file_name, 'rt')
    try:
        reader = csv.reader(csv_fh)
        headers = reader.next()
        #print("headers: %s" % headers)
        cur_frame = 0
        pos = 0
        cmd = 0
        start_frames = 0
        freq_frame = frames[0]
        next_frame = frames[1]
        i = 1
        length_frames = len(frames)
        line_nr = 0
        frame_hash = {}
        for row in reader:
            cur_frame = row[0] # frame number
            roll_pos  = row[1] # roll position
            roll_cmd  = row[3] # roll command
            pitch_pos = row[2] # pitch position
            pitch_cmd = row[4] # pitch command
            #print cur_frame + " => " + amplitude
            if (start_frames == 0 and cur_frame == freq_frame):
                start_frames = 1;
                print "going hi: " + str(line_nr) + " : " + cur_frame + " => " + next_frame
                frame_hash[freq_frame] = line_nr
            elif (start_frames != 0 and cur_frame == next_frame):
                start_frames = 0;
                frame_hash[cur_frame] = line_nr
                freq_frame = next_frame;
                if (i+1 < length_frames):
                    next_frame = frames[i+1]
                else:
                    next_frame = "0"
                i = i + 1
                print "going lo: " + str(line_nr) + " : " + cur_frame + " => " + next_frame
                start_frames = 1;
            elif (start_frames == 1):
                fr.append(int(cur_frame))
                rpos.append(float(roll_pos))
                rcmd.append(float(roll_cmd))
                ppos.append(float(pitch_pos))
                pcmd.append(float(pitch_cmd))
            line_nr = line_nr + 1
    finally:
        csv_fh.close()


def gen_charts():
    workbook  = xlsxwriter.Workbook('chart.xlsx')
    worksheet = workbook.add_worksheet()

    worksheet.write_column('A1', fr)
    worksheet.write_column('B1', rpos)
    worksheet.write_column('C1', ppos)
    worksheet.write_column('D1', rcmd)
    worksheet.write_column('E1', pcmd)

    skip = 1
    start_line = 0
    for k in freqs:
        if skip == 1:
            skip = 0
            continue;
        #print k[0], k[1], k[2]
        # k[0] start
        # k[1] end
        # k[2] freq
        n_lines = int(k[1]) - int(k[0])
        end_line = start_line + n_lines
        freq  = k[2]

        roll_chart = workbook.add_chart({'type': 'column'})
        pitch_chart = workbook.add_chart({'type': 'column'})
        
        roll_range1 =  "=Sheet1!$B$"+str(start_line)+":"+"$B$"+str(end_line)  # roll position
        roll_range2 =  "=Sheet1!$D$"+str(start_line)+":"+"$D$"+str(end_line)  # roll command
        
        pitch_range1 =  "=Sheet1!$C$"+str(start_line)+":"+"$C$"+str(end_line)+"'" # pitch position
        pitch_range2 =  "=Sheet1!$E$"+str(start_line)+":"+"$E$"+str(end_line)+"'" # pitch command
        
        roll_chart.add_series({'name' : "pos", 'values': roll_range1})
        roll_chart.add_series({'name' : "cmd", 'values': roll_range2})
        
        pitch_chart.add_series({'name' : "pos", 'values': pitch_range1})
        pitch_chart.add_series({'name' : "cmd", 'values': pitch_range2})

        roll_chart.set_title ({'name': "roll:" + str(freq)})
        roll_chart.set_x_axis({'name': 'frame'})
        roll_chart.set_y_axis({'name': 'amp'})

        pitch_chart.set_title ({'name': "pitch:" + str(freq)})
        pitch_chart.set_x_axis({'name': 'frame'})
        pitch_chart.set_y_axis({'name': 'amp'})

        worksheet.insert_chart('A7', roll_chart)
        worksheet.insert_chart('U7', pitch_chart)
        start_line = start_line + n_lines

    workbook.close()

def main():
    if (len(sys.argv) < 2):
        print "usage: freq_scan <log file> <csv file>"
        exit()

    log_file_name = sys.argv[1]
    csv_file_name = sys.argv[2]
    print "log: " + log_file_name, "csv: " + csv_file_name

    parse_log(log_file_name)
    parse_csv(csv_file_name)
    gen_charts()


if __name__ == '__main__':
    main()


