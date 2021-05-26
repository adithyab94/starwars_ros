#!/usr/bin/python

import sys, os, shutil
import smtplib
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
from email.mime.application import MIMEApplication
from email.utils import COMMASPACE, formatdate
from getpass import getpass
from time import sleep

# Student part

from_email = 'singaram.r@eleves.ec-nantes.fr'
from_name = ''          # will guess from email if empty
from_first_name = ''    # will guess from email if empty
from_passwd = ''        # will ask if empty

# end student part


# Project part
project_name = 'star_wars'    # should be the directory name + CMake project name
project_path = ''       # will find the project if empty or not valid

# ordered fields: project name / first name / name / message count
subject = 'Project {} from {} {} - status #{}/{}'

# Professor part
to_email = 'gaetan.garcia@ec-nantes.fr'
mail_delay = 15 # minutes
mail_total = 16  # number of mails

# Danger part
smtp_server = 'smtps.nomade.ec-nantes.fr'
archive_format = 'zip' # zip / gztar / tar / bztar
message_body = 'Automated message from ' + os.path.abspath(__file__)


# Do not modify below code

def extract_name(mail):
    return [s.title() for s in mail.split('@')[0].split('.')]

if not from_name:
    from_name = extract_name(from_email)[1]
if not from_first_name:
    from_first_name = extract_name(from_email)[0].split('.')[0]
if not from_passwd:
    from_passwd = getpass('Enter your password for ' + from_email + ' : ')

tar_file = '/tmp/%s-%s_%s_{}' % (project_name, from_name, from_first_name)


def init_server():
    try:
        server = smtplib.SMTP(smtp_server , 587)
        server.starttls()
        server.login(from_email, from_passwd)
        return server
    except smtplib.socket.gaierror:
        print('Could not connect to mail server, check your internet connection')
    except smtplib.SMTPAuthenticationError:
        print('Could not connect to mail server: wrong user/password')
    return None
    

# test login
if not init_server():
    sys.exit(0)
    
# Find folder if needed (assuming project name is in CMakeLists.txt)
if project_path and os.path.exists(project_path):
    project_path = [project_path]
else:
    print('Looking for project {}...'.format(project_name))
    project_path = []
    root = os.path.expanduser('~')
    ignores = ('build', 'log')
    for cur_dir, sub_dirs, files in os.walk(root):
        #print('Found directory: %s' % cur_dir)
        if '/.' in cur_dir:
            sub_dirs[:] = []
        sub_dirs[:] = [d for d in sub_dirs if d[0] != '.' and d not in ignores]
        if 'CMakeLists.txt' in files and cur_dir.endswith(project_name):
            with open(cur_dir + '/CMakeLists.txt') as f:
                cmake = f.read().lower().splitlines()
            for line in cmake:
                if 'project' in line:
                    name = line.replace('project', '').replace('(', '').replace(')', '').strip().split()[0]
                    break
            if name == project_name:
                project_path.append(cur_dir)
            
if len(project_path) == 0:
    print('  Could not find project named {}'.format(project_name))
    sys.exit(0)
    
if len(project_path) == 1:
    print('  Found project @ {}'.format(project_path[0]))
    r = raw_input('  Confirm location [Y/n]?: ')
    if r == 'n':
        sys.exit(0)
    project_path = project_path[0]
else:
    print('  Found several project locations:\n')
    for i,s in enumerate(project_path):
        print('  ({}) @ {}'.format(i+1, s))
    r = raw_input('\n  Select location to use (enter to cancel): ')
    if r.isdigit() and 0 < int(r) <= len(project_path):
        project_path = project_path[int(r)-1]
    else:
        sys.exit(0)
        
        
# main loop
to_header = '{} {} <{}>'.format(*(extract_name(to_email) + [to_email]))
from_header = '{} {} <{}>'.format(from_first_name, from_name, from_email)

for status_count in range(1,mail_total+1):
    
    sleep(mail_delay * 60)
    
    image = shutil.make_archive(tar_file.format(status_count), archive_format, project_path)
    
    # build message
    msg = MIMEMultipart()
    msg['From'] = from_header
    msg['To'] = to_header
    msg['Subject'] = subject.format(project_name, from_first_name, from_name, status_count, mail_total)
    msg['Date'] = formatdate(localtime=True)
    
    msg.attach(MIMEText(message_body, 'plain'))
    
    with open(image, 'rb') as f:
        attached_file = MIMEApplication(f.read(), _subtype=archive_format) 
        attached_file.add_header('content-disposition', 'attachment', filename=os.path.basename(image))
    msg.attach(attached_file)
    
    # send message
    server = init_server()
    if server:
        print('Sending "{}" to {} ...'.format(msg['Subject'], to_email))
        server.sendmail(from_email, to_email, msg.as_string())
        server.close()
        
