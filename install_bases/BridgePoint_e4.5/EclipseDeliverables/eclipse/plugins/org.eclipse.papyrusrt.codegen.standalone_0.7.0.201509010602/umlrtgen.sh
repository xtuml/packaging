#!/bin/sh

# PLUGIN_DIR is the root directory of plugin dependencies, i.e. the 'plugins' directory of the target
# eclipse platform, or just a directory that contains all required Eclipse jar files
PLUGIN_DIR=$1

# SRC_DIR is the root of the development directory containing the binaries of the development
# version of the code generator.
SRC_DIR=$2

JAVA_VM=$3

if [ -z "$PLUGIN_DIR" -o -z "$SRC_DIR" ]; then
    echo "Usage $0 <plugin-dir> <src-dir> [<java-vm>]"
    exit 1
fi

shift
shift

if [ -z "$JAVA_VM" ]; then
    JAVA_VM=java
else
    shift
fi

# Initialize the class path with at least the standalone generator.
cp=`dirname $0`:`dirname $0`/lib/commons-cli-1.2.jar

# Add the given class location (folder of .class files or .jar file) to the
# classpath that will be used.
update_classpath() {
    to_add=$1
    if [ -z "$to_add" ]; then
        echo Nothing to add
        exit 1
    fi

    if [ -z "$cp" ]; then
        cp=$to_add
    else
        cp=$cp:$to_add
    fi
}

# Use the list of base paths to find the full path to the given jar file.
find_jar_for() {

    # Search the development workspace first.  Look for a folder with the exact
    # name.  If found put the contained bin folder into the classpath.
    project=`find $SRC_DIR -name .git -prune -o -type d -name ${1} -print`
    if [ -n "$project" -a -d $project/bin ]; then
        update_classpath $project/bin
        return
    fi

    # Otherwise look for a jar file in the reference plugins folder.  Allow for the
    # _<version>.jar, e.g., org.eclipse.uml2.uml_5.0.1.v20140910-1354.jar.
    jar=`find $SRC_DIR $PLUGIN_DIR -name ${1}_\*.jar`
    if [ -n "$jar" ]; then
        update_classpath $jar
    fi
}

# Use the list of base paths to find the full path to the given plugin dir.
find_plugindir_for() {

    # Look for a plugin dir in the reference plugins folder.  Allow for the
    # _<version>, e.g., org.eclipse.papyrusrt.codegen.standalone_0.7.0.v201508261544
    plugindir=`find $SRC_DIR $PLUGIN_DIR -name ${1}_\*`
    if [ -n "$plugindir" -a -d $plugindir ]; then
        update_classpath $plugindir
    fi
}

# The XtUMLRT dependencies

find_jar_for org.eclipse.papyrusrt.xtumlrt.common.model
find_jar_for org.eclipse.papyrusrt.xtumlrt.platform.cpp.model
find_jar_for org.eclipse.papyrusrt.xtumlrt.platform.model
find_jar_for org.eclipse.papyrusrt.xtumlrt.umlrt.model

# The UML-RT plugins
find_jar_for org.eclipse.papyrusrt.codegen
find_jar_for org.eclipse.papyrusrt.codegen.cpp
find_jar_for org.eclipse.papyrusrt.codegen.cpp.rts
find_jar_for org.eclipse.papyrusrt.codegen.cpp.statemachines.flat
find_jar_for org.eclipse.papyrusrt.codegen.cpp.structure
find_jar_for org.eclipse.papyrusrt.codegen.lang
find_jar_for org.eclipse.papyrusrt.codegen.lang.cpp
find_jar_for org.eclipse.papyrusrt.codegen.statemachines.flat
find_jar_for org.eclipse.papyrusrt.codegen.statemachines.flat.model
find_jar_for org.eclipse.papyrusrt.codegen.utils
find_jar_for org.eclipse.papyrusrt.codegen.xtumlrt.trans
find_jar_for org.eclipse.papyrusrt.codegen.papyrus
find_jar_for org.eclipse.papyrusrt.releng
find_jar_for org.eclipse.papyrusrt.umlrt.profile
find_jar_for org.eclipse.papyrusrt.rts
find_plugindir_for org.eclipse.papyrusrt.codegen.standalone

# The dependencies
find_jar_for org.eclipse.uml2.uml
find_jar_for org.eclipse.uml2.uml.resources
find_jar_for org.eclipse.emf.ecore
find_jar_for org.eclipse.emf.common
find_jar_for org.eclipse.ocl
find_jar_for org.eclipse.equinox.registry
find_jar_for org.eclipse.uml2.types
find_jar_for org.eclipse.emf.ecore.xmi
find_jar_for org.eclipse.core.runtime
find_jar_for org.eclipse.osgi
find_jar_for org.eclipse.papyrus.cpp.profile
find_jar_for org.eclipse.equinox.common
find_jar_for org.eclipse.uml2.common
find_jar_for org.eclipse.uml2.uml.profile.standard
find_jar_for org.eclipse.emf.mapping.ecore2xml
find_jar_for org.eclipse.xtext.xbase.lib
find_jar_for com.google.guava
find_jar_for org.apache.commons.cli

update_classpath ./bin
update_classpath $PLUGIN_DIR

echo $JAVA_VM -cp $cp org.eclipse.papyrusrt.codegen.standalone.StandaloneUMLRTCodeGenerator -p $PLUGIN_DIR $@
$JAVA_VM -cp $cp org.eclipse.papyrusrt.codegen.standalone.StandaloneUMLRTCodeGenerator -p $PLUGIN_DIR $@
