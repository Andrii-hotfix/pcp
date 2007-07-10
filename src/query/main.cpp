/*
 * Copyright (c) 2007, Nathan Scott.  All Rights Reserved.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 * 
 * Contact information: Nathan Scott, nathans At debian DoT org
 */

#include <qapplication.h>
#include <qlayout.h>
#include <qframe.h>
#include <qlabel.h>
#include <qfile.h>
#include <errno.h>
#include "kmquery.h"

static char usage[] =
    "Usage: kmquery [options] [message...]\n\n"
    "Options:\n"
    "  -c              center the window on the display\n"
    "  -center         display in the center (alias for -c)\n"
    "  -nearmouse      pop up window near the mouse cursor\n"
    "  -b button       create a button with the label button\n"
    "  -B button       create the default button with the label button\n"
    "  -default button sets named button as the default button\n"
    "  -buttons string comma-separated list of label:exitcode\n"
    "  -h | -? | -help display this usage message\n"
    "  -t string       add string to the message displayed\n"
    "  -file filename  read message from file, \"-\" for stdin\n"
    "  -icon icontype  dialog type: info, error, question, warning, critical,\n"
    "                               host, or archive\n"
    "  -header title   set window title\n"
    "  -useslider      always display a text box slider\n"
    "  -noslider       do not display a text box slider\n"
    "  -noframe        do not display a frame around the text box\n"
    "  -print          print the button label when selected\n"
    "  -noprint        do not print the button label when selected\n"
    "  -timeout secs   exit with status 0 after \"secs\" seconds\n"
    "  -exclusive      do not allow mouse/button presses until dismissed\n";

char *getoption(int argc, char **argv)
{
    static int index;

    if (index >= argc)
	return NULL;
    return argv[++index];
}

char *catoption(char *prefix, char *option, int total)
{
    prefix = (char *)realloc(prefix, total);
    if (prefix) {
	strncat(prefix, " ", 2);
	strncat(prefix, option, total);
    }
    return prefix;
}

char *getoptions(int argc, char **argv, char *arg)
{
    int length = strlen(arg) + 1;
    char *string = strndup(arg, length);
    while (string && (arg = getoption(argc, argv)) != NULL) {
	length += 1 + strlen(arg) + 1;
	string = catoption(string, arg, length);
    }
    if (!string) {
	fputs("Insufficient memory for buffering message\n", stderr);
	exit(1);
    }
    return string;
}

int main(int argc, char ** argv)
{
    char *option;
    char *filename = NULL;
    char *defaultname = NULL;
    int errflag = 0;
    int printflag = 1;
    int inputflag = 0;
    int centerflag = 0;
    int noframeflag = 0;
    int nosliderflag = 0;
    int nearmouseflag = 0;
    int usesliderflag = 0;
    int exclusiveflag = 0;

    QApplication a(argc, argv);

    while ((option = getoption(argc, argv)) != NULL) {
	if (strcmp(option, "-c") == 0 || strcmp(option, "-center") == 0) {
	    centerflag = 1;
	}
	else if (strcmp(option, "-nearmouse") == 0) {
	    nearmouseflag = 1;
	}
	else if (strcmp(option, "-b") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -b option requires an argument\n");
		errflag++;
	    }
	    KmQuery::addButton(option, FALSE, 0);
	}
	else if (strcmp(option, "-B") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -B option requires an argument\n");
		errflag++;
	    }
	    KmQuery::addButton(option, TRUE, 0);
	}
	else if (strcmp(option, "-default") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -default option requires an argument\n");
		errflag++;
	    }
	    else defaultname = option;
	}
	else if (strcmp(option, "-buttons") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -buttons option requires an argument\n");
		errflag++;
	    }
	    KmQuery::addButtons(option);
	}
	else if (strcmp(option, "-t") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -B option requires an argument\n");
		errflag++;
	    }
	    else if (filename) {
		fprintf(stderr, "The -file and -t options are incompatible\n");
		errflag++;
	    }
	    else KmQuery::addMessage(option);
	}
	else if (strcmp(option, "-file") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -file option requires an argument\n");
		errflag++;
	    }
	    else if (KmQuery::messageCount()) {
		fprintf(stderr, "The -file and -t options are incompatible\n");
		errflag++;
	    }
	    else filename = option;
	}
	else if (strcmp(option, "-icon") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -icon option requires an argument\n");
		errflag++;
	    }
	    else if (KmQuery::setIcontype(option) < 0) {
		fprintf(stderr, "Unknown icon type - %s\n", option);
		errflag++;
	    }
	}
	else if (strcmp(option, "-header") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -header option requires an argument\n");
		errflag++;
	    }
	    else KmQuery::setTitle(option);
	}
	else if (strcmp(option, "-input") == 0) {
	    inputflag = 1;
	}
	else if (strcmp(option, "-noframe") == 0) {
	    noframeflag = 1;
	}
	else if (strcmp(option, "-useslider") == 0) {
	    if (nosliderflag) {
		fprintf(stderr,
		    "The -useslider and -noslider options are incompatible\n");
		errflag++;
	    }
	    else usesliderflag = 1;
	}
	else if (strcmp(option, "-noslider") == 0) {
	    if (usesliderflag) {
		fprintf(stderr,
		    "The -useslider and -noslider options are incompatible\n");
		errflag++;
	    }
	    else nosliderflag = 1;
	}
	else if (strcmp(option, "-timeout") == 0) {
	    if ((option = getoption(argc, argv)) == NULL) {
		fprintf(stderr, "The -timeout option requires an argument\n");
		errflag++;
	    }
	    else if (KmQuery::setTimeout(option) < 0) {
		fprintf(stderr, "'%s' is not a positive non-zero timeout\n",
			option);
		errflag++;
	    }
	}
	else if (strcmp(option, "-print") == 0) {
	    printflag = 1;
	}
	else if (strcmp(option, "-noprint") == 0) {
	    printflag = 0;
	}
	else if (strcmp(option, "-exclusive") == 0) {
	    exclusiveflag = 1;
	}
	else if (strcmp(option, "-?") == 0 || strcmp(option, "-help") == 0 ||
		 strcmp(option, "-h") == 0 || strcmp(option, "--help") == 0) {
	    errflag++;
	}
	else {
	    KmQuery::addMessage(getoptions(argc, argv, option));
	}
    }

    if (errflag) {
	fprintf(stderr, usage);
	exit(1);
    }

    if (defaultname)
	KmQuery::setDefaultButton(option);

    if (filename) {
	QTextStream *stream;
	QFile *file = NULL;

	if (strcmp(filename, "-") == 0)
	    stream = new QTextStream(stdin, IO_ReadOnly);
	else {
	    file = new QFile(filename);
	    if (!file->open(IO_ReadOnly)) {
		fprintf(stderr, "Cannot open %s: %s\n", filename,
			strerror(errno));
		exit(1);
	    }
	    stream = new QTextStream(file);
	}
	while (!stream->atEnd()) {
	    if ((option = strdup(stream->readLine().ascii())) == NULL) {
		fputs("Insufficient memory reading message stream\n", stderr);
		exit(1);
	    }
	    KmQuery::addMessage(option);
	}
	if (file)
	    delete file;
	delete stream;
    }

    if (!KmQuery::buttonCount())
	KmQuery::addButton("Continue", TRUE, 0);

    KmQuery q(centerflag, nearmouseflag, inputflag, printflag, noframeflag,
	      nosliderflag, usesliderflag, exclusiveflag);
    return q.exec();
}
