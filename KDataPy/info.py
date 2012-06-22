"""
 __  _ ___    ____ ______  ____ 
|  |/ ]   \  /    |      |/    |
|  ' /|    \|  o  |      |  o  |
|    \|  D  |     |_|  |_|     |
|     \     |  _  | |  | |  _  |
|  .  |     |  |  | |  | |  |  |
|__|\_|_____|__|__| |__| |__|__|Py

      %s
"""

from collections import namedtuple


_version_info_base = namedtuple('version_info',
                                ['major',
                                 'minor',
                                 'micro',
                                 'releaselevel'])


class version_info(_version_info_base):

    def __repr__(self):

        return 'kdata.%s' % super(version_info, self).__repr__()

    def __str__(self):

        if self.releaselevel != 'final':
            return '%s.%s.%s-%s%s' % (self.major,
                                      self.minor,
                                      self.micro,
                                      self.releaselevel[0])
        return '%s.%s.%s' % (self[:3])


__version_info__ = version_info(4, 7, 0, 'final')
__version__ = str(__version_info__)
__url__ = 'https://edwdev-ik.fzk.de/'
__repo_url__ = 'https://edwdev-ik.fzk.de/wsvn/EDELWEISS/analysis/kdata/'
__download_url__ = ('https://edwdev-ik.fzk.de/wsvn/EDELWEISS/analysis/kdata/tags/%d.%d/?op=dl&rev=0&isdir=1') % (__version_info__.major, __version_info__.minor)
__doc__ %= __version__