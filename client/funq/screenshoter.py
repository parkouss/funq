# -*- coding: utf-8 -*-

# Copyright: SCLE SFE
# Contributor: Julien Pagès <j.parkouss@gmail.com>
#
# This software is a computer program whose purpose is to test graphical
# applications written with the QT framework (http://qt.digia.com/).
#
# This software is governed by the CeCILL v2.1 license under French law and
# abiding by the rules of distribution of free software.  You can  use,
# modify and/ or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL v2.1 license and that you accept its terms.

"""
This module allows to take screenshots and to save them as PNG image
automatically.
"""

import os
import itertools
import logging
import codecs

LOG = logging.getLogger('funq.screenshoter')


class ScreenShoter(object):  # pylint: disable=R0903

    """
    Object that can take screenshots.
    """

    def __init__(self, working_folder):
        self.working_folder = os.path.realpath(working_folder)
        self.txt_file_path = os.path.join(self.working_folder, 'images.txt')
        self.counter = itertools.count()

        if os.path.isfile(self.txt_file_path):
            os.unlink(self.txt_file_path)

    def take_screenshot(self, funqclient, longname):
        """
        Take a screenshot of the given application, associating **longname**
        in the generated images.txt file.
        """
        if not os.path.isdir(self.working_folder):
            os.makedirs(self.working_folder)

        bname = '{0}.png'.format(self.counter.next())
        fname = os.path.join(self.working_folder, bname)

        try:
            funqclient.take_screenshot(fname, 'PNG')
        except (SystemExit, KeyboardInterrupt):
            raise
        except:
            LOG.exception(u"impossible de prendre un screenshot pour"
                          u" %s", longname)
            return

        with codecs.open(self.txt_file_path, "a", "utf-8") as f:
            f.write(u"{0}: {1}\n".format(bname, longname))
