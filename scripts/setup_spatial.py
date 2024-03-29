#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
   Copyright (c) 2012 The Ohio State University.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

"""


import os
import subprocess
import sys
import setup_integration_1
import setup_integration_2

CURRENT_DIR = os.getcwd()
FRONTEND_DIR = 'SpatialSQL2XML'
BACKEND_DIR = 'XML2MapReduce'
EXEC_DIR = 'bin'


def install_frontend():
    if os.path.exists(CURRENT_DIR + '/' + EXEC_DIR) is False:
        os.mkdir(CURRENT_DIR + '/' + EXEC_DIR)
    print '--------------------------------------------------------------------'
    print 'Compiling front-end SQL queries lexer and parser ...'
    print '--------------------------------------------------------------------'
    os.chdir(CURRENT_DIR + '/' + FRONTEND_DIR)
    subprocess.check_call('tar xzvf antlr-3.3.tar.gz', shell=True)
    print '--------------------------------------------------------------------'
    print 'Generating lexer and parser ...'
    
    #print "java -jar antlr-3.3/lib/antlr-3.3-complete.jar YSmart.g" #was in YSmart
    #subprocess.check_call('java -jar antlr-3.3/lib/antlr-3.3-complete.jar YSmart.g', shell=True) #was in YSmart
    
    print "java -Xms32m -Xmx512m -jar antlr-3.3/lib/antlr-3.3-complete.jar YSmartSpatial.g"
    print '--------------------------------------------------------------------'
    subprocess.check_call('java -Xms32m -Xmx512m -jar antlr-3.3/lib/antlr-3.3-complete.jar YSmartSpatial.g', shell=True)
    
    os.chdir(CURRENT_DIR + '/' + FRONTEND_DIR + '/antlr-3.3/runtime/C/dist')
    subprocess.check_call(['tar', 'xzvf', 'libantlr3c-3.1.4-SNAPSHOT.tar.gz', '-C', CURRENT_DIR + '/' + FRONTEND_DIR])

    os.chdir(CURRENT_DIR + '/' + FRONTEND_DIR + '/libantlr3c-3.1.4-SNAPSHOT')
    subprocess.check_call('./configure --prefix=/tmp/antrl_runtime_install_dir', shell=True)
    subprocess.check_call('make install', shell=True)

    os.chdir(CURRENT_DIR + '/' + FRONTEND_DIR)
    print '--------------------------------------------------------------------'
    print 'Compiling ...'
    # was in YSmart
    #print 'gcc -o YSmartFront.exe YSmartMain.c YSmartLexer.c YSmartParser.c /tmp/antrl_runtime_install_dir/lib/libantlr3c.a -I . -I /tmp/antrl_runtime_install_dir/include/' 
    #print '--------------------------------------------------------------------'
    #subprocess.check_call(['gcc', '-m32', '-o', 'YSmartFront.exe', 'YSmartMain.c', 'YSmartLexer.c', 'YSmartParser.c', \
    #               '/tmp/antrl_runtime_install_dir/lib/libantlr3c.a', '-I', '.', '-I', '/tmp/antrl_runtime_install_dir/include/'])
    
    print 'gcc -m32 -o YSmartSpatialFront.exe  YSmartSpatialMain.c YSmartSpatialLexer.c YSmartSpatialParser.c /tmp/antrl_runtime_install_dir/lib/libantlr3c.a -I . -I /tmp/antrl_runtime_install_dir/include' 
    print '--------------------------------------------------------------------'    
    subprocess.check_call(['gcc', '-m32', '-o', 'YSmartSpatialFront.exe', 'YSmartSpatialMain.c', 'YSmartSpatialLexer.c', 'YSmartSpatialParser.c', \
                       '/tmp/antrl_runtime_install_dir/lib/libantlr3c.a', '-I', '.', '-I', '/tmp/antrl_runtime_install_dir/include/'])    
    
    
    
    
    #subprocess.check_call(['mv', 'YSmartFront.exe', CURRENT_DIR + '/' + EXEC_DIR]) #was in YSmart
    subprocess.check_call(['mv', 'YSmartSpatialFront.exe', CURRENT_DIR + '/' + EXEC_DIR]) 
    
    print '--------------------------------------------------------------------'
    print 'Front-end SQL queries lexer and parser have been compiled'
    print '--------------------------------------------------------------------'
    
    print '--------------------------------------------------------------------'
    print 'Preparing setup for JNI integration with RESQUE'
    print '--------------------------------------------------------------------'
    #BY DEFAULT, VERSION 1 OF RESQUE IS USED, YOU CAN ANY TIME SWITCH TO VERSION 2 SEE READE STEP 1)
    setup_integration_1.setup_integrate_install()   #version1
    # setup_integration_2.setup_integrate_install() #version2

    print '--------------------------------------------------------------------'
    print 'Preparing back-end hadoop job generator'
    print '--------------------------------------------------------------------'

    #print 'TODO: make a soft link of job generation program in ./bin/'

def install_backend():
    os.chdir(CURRENT_DIR)
    subprocess.check_call(['ln', '-sf', BACKEND_DIR + '/main.py', EXEC_DIR + '/main.py'])


    

def uninstall():
    print '--------------------------------------------------------------------'
    print 'Cleaning front-end SQL queries lexer and parser ...'
    print '--------------------------------------------------------------------'
    os.chdir(CURRENT_DIR + '/' + FRONTEND_DIR)
    subprocess.check_call(['rm', '-rf', 'antlr-3.3', 'libantlr3c-3.1.4-SNAPSHOT', '/tmp/antrl_runtime_install_dir'])
    
    #subprocess.check_call(['rm', 'YSmartLexer.h', 'YSmartLexer.c', 'YSmartParser.h', 'YSmartParser.c', 'YSmart.tokens']) #was in YSmart
    subprocess.check_call(['rm', 'YSmartSpatialLexer.h', 'YSmartSpatialLexer.c', 'YSmartSpatialParser.h', 'YSmartSpatialParser.c', 'YSmartSpatial.tokens'])
    
    print '--------------------------------------------------------------------'
    print 'Cleaning JNI integration with RESQUE'
    print '--------------------------------------------------------------------'
    
    #BY DEFAULT, VERSION 1 OF RESQUE IS USED, YOU CAN ANY TIME SWITCH TO VERSION 2 SEE READE STEP 1)
    setup_integration_1.setup_integrate_uninstall()  #version 1
    # setup_integration_2.setup_integrate_uninstall() #version2
    
    
    print '--------------------------------------------------------------------'
    print 'Cleaning ' + EXEC_DIR + ' ...'
    print '--------------------------------------------------------------------'
    os.chdir(CURRENT_DIR)
    subprocess.check_call(['rm', '-rf', './' + EXEC_DIR])
    
    print '--------------------------------------------------------------------'
    print 'Done'
    print '--------------------------------------------------------------------'

def print_usage():
    # print 'usage: ./setup.py install/uninstall' # was in YSmart
    print 'usage: ./setup_spatial.py install/uninstall'

def main():
    if (len(sys.argv) != 2):
        print_usage()
        sys.exit(0)
    if (sys.argv[1] == 'install'):
        install_frontend()
#       install_backend()
    elif (sys.argv[1] == 'uninstall'):
        uninstall()
    else:
        print_usage()
        sys.exit(0)
    
if __name__ == "__main__":
    main()





