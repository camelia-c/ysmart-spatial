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
import commands

CURRENT_DIR = os.getcwd()
FRONTEND_DIR = 'SpatialSQL2XML'
BACKEND_DIR = 'XML2MapReduce'
EXEC_DIR = 'bin'
BOOST_DIR= 'boost_library'

#please modify the following 4 variables according to your own paths, see README
SPATIAL_SETUP_FOLDER_LIB="/home/camelia/Documents/gsoc_emory/resque/usr/lib"
SPATIAL_SETUP_FOLDER_INCLUDE="/home/camelia/Documents/gsoc_emory/resque/usr/include"
CGAL_SETUP_FOLDER_LIB="/home/camelia/Documents/gsoc_emory/resque/CGAL-3.8/lib"
CGAL_SETUP_FOLDER_INCLUDE="/home/camelia/Documents/gsoc_emory/resque/CGAL-3.8/include"

def setup_integrate_install():
    print 'Compiling for JNI with RESQUE version 2 (SpatialIndex,BOOST,CGAL based)...'
    
    os.chdir(CURRENT_DIR + '/' + BACKEND_DIR)
    print 'chdir to...',os.getcwd()
    #compile the java code for integration
    version = "0.21.0"
    if "HADOOP_HOME" in os.environ:
        version = commands.getoutput("$HADOOP_HOME/bin/hadoop version").split("\n")[0].split(" ")[1]
        print 'Your HADOOP VERSION IS:',version
    print 'javac -classpath $HADOOP_HOME/hadoop-common-'+version+'.jar:$HADOOP_HOME/hadoop-hdfs-'+version+'.jar:$HADOOP_HOME/hadoop-mapred-'+version+'.jar  ReducerRESQUE.java -d .'
    print '--------------------------------------------------------------------'    
    os.system('javac -classpath $HADOOP_HOME/hadoop-common-'+version+'.jar:$HADOOP_HOME/hadoop-hdfs-'+version+'.jar:$HADOOP_HOME/hadoop-mapred-'+version+'.jar  ReducerRESQUE.java -d .')
    
    #print 'javac -classpath /usr/lib/hadoop-0.20/hadoop-core-0.20.2-cdh3u3.jar  ReducerRESQUE.java -d .' 
    #print '--------------------------------------------------------------------'    
    #subprocess.check_call(['javac', '-classpath', '/usr/lib/hadoop-0.20/hadoop-core-0.20.2-cdh3u3.jar', 'ReducerRESQUE.java', '-d', '.'])    
    
    #generate the header for integration
    print 'javah -classpath '+os.getcwd()+' edu.osu.cse.ysmart.testquery.ReducerRESQUE' 
    print '--------------------------------------------------------------------'    
    subprocess.check_call(['javah', '-classpath', os.getcwd(), 'edu.osu.cse.ysmart.testquery.ReducerRESQUE'])   
    
    #generate the library for integration, version 2
    print 'g++  -o libReducerRESQUE.so -shared -Wl,-soname,libReducerRESQUE.so -L'+SPATIAL_SETUP_FOLDER_LIB+' -I'+SPATIAL_SETUP_FOLDER_INCLUDE+' -L'+CGAL_SETUP_FOLDER_LIB+' -I/java/include -I/usr/lib/jvm/jdk1.6.0/include/linux -I /usr/lib/jvm/jdk1.6.0/include/  -I'+CURRENT_DIR + '/' +BOOST_DIR+' -I'+CGAL_SETUP_FOLDER_INCLUDE+' -I'+CURRENT_DIR + '/' +BACKEND_DIR+'  '+os.getcwd()+'/ReducerRESQUE_2.cpp -static -lc -lpthread -lspatialindex -lCGAL' 
    print '--------------------------------------------------------------------'    
    subprocess.check_call(['g++', '-olibReducerRESQUE.so', '-shared','-Wl,-soname,libReducerRESQUE.so','-L'+SPATIAL_SETUP_FOLDER_LIB,'-I'+SPATIAL_SETUP_FOLDER_INCLUDE,'-L'+CGAL_SETUP_FOLDER_LIB,'-I/java/include','-I/usr/lib/jvm/jdk1.6.0/include/linux','-I/usr/lib/jvm/jdk1.6.0/include/','-I'+CURRENT_DIR + '/' +BOOST_DIR,'-I'+CGAL_SETUP_FOLDER_INCLUDE,'-I'+CURRENT_DIR + '/' +BACKEND_DIR,''+os.getcwd()+'/ReducerRESQUE_2.cpp','-static','-lc','-lpthread','-lspatialindex','-lCGAL'])  
    
    
    #copy the library for integration in the bin directory
    subprocess.check_call(['cp', 'libReducerRESQUE.so', CURRENT_DIR + '/' + EXEC_DIR])
    
    #export the path to the library
    # LD_LIBRARY_PATH=CURRENT_DIR + '/' +EXEC_DIR:SPATIAL_SETUP_FOLDER_LIB:CGAL_SETUP_FOLDER_LIB
    # export LD_LIBRARY_PATH 

    
    os.chdir(CURRENT_DIR)
    print 'chdir to...',os.getcwd()
    
    return

def setup_integrate_uninstall():
    print 'uninstalling.....'
    os.chdir(CURRENT_DIR + '/' + BACKEND_DIR)
    print 'chdir to...',os.getcwd()
    
    print 'rm edu_osu_cse_ysmart_testquery_ReducerRESQUE.h'
    subprocess.check_call(['rm', 'edu_osu_cse_ysmart_testquery_ReducerRESQUE.h'])
    print 'rm libReducerRESQUE.so'
    subprocess.check_call(['rm', 'libReducerRESQUE.so'])
    
    print 'rm -rf ./edu'
    #subprocess.check_call(['rm', '-rf', './edu'])
    os.chdir(CURRENT_DIR)
    
    return 

def print_usage():
    print 'usage: ./setup_integration_2.py install/uninstall'

def main():
    if (len(sys.argv) != 2):
        print_usage()
        sys.exit(0)
    if (sys.argv[1] == 'install'):
        setup_integrate_install()
    elif (sys.argv[1] == 'uninstall'):
        setup_integrate_uninstall()
    else:
        print_usage()
        sys.exit(0)
    
if __name__ == "__main__":
    main()
