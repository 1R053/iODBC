/*
 *  SQLRemoveDSNFromIni.c
 *
 *  $Id$
 *
 *  Remove a DSN
 *
 *  The iODBC driver manager.
 *
 *  Copyright (C) 2001 by OpenLink Software <iodbc@openlinksw.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iodbc.h>
#include <iodbcinst.h>

#include "inifile.h"
#include "misc.h"
#include "iodbc_error.h"

extern BOOL ValidDSN(LPCSTR);

BOOL
RemoveDSNFromIni (LPCSTR lpszDSN)
{
  char szBuffer[4096];
  BOOL retcode = FALSE;
  PCONFIG pCfg;

  /* Check dsn */
  if (!lpszDSN || !ValidDSN (lpszDSN) || !STRLEN (lpszDSN))
    {
      PUSH_ERROR (ODBC_ERROR_INVALID_DSN);
      goto quit;
    }

  if (_iodbcdm_cfg_search_init (&pCfg, "odbc.ini", TRUE))
    {
      PUSH_ERROR (ODBC_ERROR_REQUEST_FAILED);
      goto quit;
    }

  if (strcmp (lpszDSN, "Default"))
    {
      /* deletes a DSN from [ODBC data sources] section */
#ifdef WIN32
      _iodbcdm_cfg_write (pCfg, "ODBC 32 bit Data Sources", (LPSTR) lpszDSN,
	  NULL);
#else
      _iodbcdm_cfg_write (pCfg, "ODBC Data Sources", (LPSTR) lpszDSN, NULL);
#endif
    }

  /* deletes the DSN section in odbc.ini */
  _iodbcdm_cfg_write (pCfg, (LPSTR) lpszDSN, NULL, NULL);

  if (_iodbcdm_cfg_commit (pCfg))
    {
      PUSH_ERROR (ODBC_ERROR_REQUEST_FAILED);
      goto done;
    }

  retcode = TRUE;

done:
  _iodbcdm_cfg_done (pCfg);

quit:
  return retcode;
}


BOOL INSTAPI
SQLRemoveDSNFromIni (LPCSTR lpszDSN)
{
  BOOL retcode = FALSE;

  CLEAR_ERROR ();

  switch (configMode)
    {
    case ODBC_USER_DSN:
      wSystemDSN = USERDSN_ONLY;
      retcode = RemoveDSNFromIni (lpszDSN);
      goto quit;

    case ODBC_SYSTEM_DSN:
      wSystemDSN = SYSTEMDSN_ONLY;
      retcode = RemoveDSNFromIni (lpszDSN);
      goto quit;

    case ODBC_BOTH_DSN:
      wSystemDSN = USERDSN_ONLY;
      retcode = RemoveDSNFromIni (lpszDSN);
      if (!retcode)
	{
	  CLEAR_ERROR ();
	  wSystemDSN = SYSTEMDSN_ONLY;
	  retcode = RemoveDSNFromIni (lpszDSN);
	}
      goto quit;
    };

  PUSH_ERROR (ODBC_ERROR_GENERAL_ERR);
  goto quit;

quit:
  wSystemDSN = USERDSN_ONLY;
  configMode = ODBC_BOTH_DSN;
  return retcode;
}