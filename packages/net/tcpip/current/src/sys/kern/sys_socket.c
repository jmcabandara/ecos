//==========================================================================
//
//      sys/kern/sys_socket.c
//
//     
//
//==========================================================================
// ####BSDALTCOPYRIGHTBEGIN####                                             
// -------------------------------------------                              
// Portions of this software may have been derived from OpenBSD             
// or other sources, and if so are covered by the appropriate copyright     
// and license included herein.                                             
// -------------------------------------------                              
// ####BSDALTCOPYRIGHTEND####                                               
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


/*	$OpenBSD: sys_socket.c,v 1.3 1997/08/31 20:42:23 deraadt Exp $	*/
/*	$NetBSD: sys_socket.c,v 1.13 1995/08/12 23:59:09 mycroft Exp $	*/

/*
 * Copyright (c) 1982, 1986, 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)sys_socket.c	8.1 (Berkeley) 6/10/93
 */

#include <sys/param.h>
#ifdef __ECOS
#include <cyg/io/file.h>
#else
#include <sys/systm.h>
#include <sys/file.h>
#include <sys/proc.h>
#endif
#include <sys/mbuf.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/ioctl.h>
#ifndef __ECOS
#include <sys/stat.h>
#endif

#include <net/if.h>
#include <net/route.h>

struct	fileops socketops =
    { soo_read, soo_write, soo_ioctl, soo_select, soo_close };

#ifdef __ECOS
/* ARGSUSED */
int
soo_read(struct file *fp, struct uio *uio)
#else
/* ARGSUSED */
int
soo_read(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
#endif
{

	return (soreceive((struct socket *)fp->f_data, (struct mbuf **)0,
		uio, (struct mbuf **)0, (struct mbuf **)0, (int *)0));
}

#ifdef __ECOS
int
soo_write(struct file *fp, struct uio *uio)
#else
/* ARGSUSED */
int
soo_write(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
#endif
{

	return (sosend((struct socket *)fp->f_data, (struct mbuf *)0,
		uio, (struct mbuf *)0, (struct mbuf *)0, 0));
}

#ifdef __ECOS
int
soo_ioctl(struct file *fp, CYG_ADDRWORD cmd, CYG_ADDRWORD data)
#else
int
soo_ioctl(fp, cmd, data, p)
	struct file *fp;
	u_long cmd;
	register caddr_t data;
	struct proc *p;
#endif
{
	register struct socket *so = (struct socket *)fp->f_data;
#ifdef __ECOS
        void *p = 0;
#endif

	switch (cmd) {

	case FIONBIO:
		if (*(int *)data)
			so->so_state |= SS_NBIO;
		else
			so->so_state &= ~SS_NBIO;
		return (0);

	case FIOASYNC:
		if (*(int *)data) {
			so->so_state |= SS_ASYNC;
			so->so_rcv.sb_flags |= SB_ASYNC;
			so->so_snd.sb_flags |= SB_ASYNC;
		} else {
			so->so_state &= ~SS_ASYNC;
			so->so_rcv.sb_flags &= ~SB_ASYNC;
			so->so_snd.sb_flags &= ~SB_ASYNC;
		}
		return (0);

	case FIONREAD:
		*(int *)data = so->so_rcv.sb_cc;
		return (0);

#ifndef __ECOS
	case SIOCSPGRP:
		so->so_pgid = *(int *)data;
		so->so_siguid = p->p_cred->p_ruid;
		so->so_sigeuid = p->p_ucred->cr_uid;
		return (0);

	case SIOCGPGRP:
		*(int *)data = so->so_pgid;
		return (0);
#endif

	case SIOCATMARK:
		*(int *)data = (so->so_state&SS_RCVATMARK) != 0;
		return (0);
	}
	/*
	 * Interface/routing/protocol specific ioctls:
	 * interface and routing ioctls should have a
	 * different entry since a socket's unnecessary
	 */
	if (IOCGROUP(cmd) == 'i')
		return (ifioctl(so, (u_long)cmd, (caddr_t)data, p));
	if (IOCGROUP(cmd) == 'r')
		return (rtioctl((u_long)cmd, (caddr_t)data, p));
	return ((*so->so_proto->pr_usrreq)(so, PRU_CONTROL, 
	    (struct mbuf *)cmd, (struct mbuf *)data, (struct mbuf *)0));
}

#ifdef __ECOS
int
soo_select(struct file *fp, int which)
#else
int
soo_select(fp, which, p)
	struct file *fp;
	int which;
	struct proc *p;
#endif
{
	register struct socket *so = (struct socket *)fp->f_data;
	register int s = splsoftnet();
#ifdef __ECOS
        void *p = 0;
#endif

	switch (which) {

	case FREAD:
		if (soreadable(so)) {
			splx(s);
			return (1);
		}
		selrecord(p, &so->so_rcv.sb_sel);
		so->so_rcv.sb_flags |= SB_SEL;
		break;

	case FWRITE:
		if (sowriteable(so)) {
			splx(s);
			return (1);
		}
		selrecord(p, &so->so_snd.sb_sel);
		so->so_snd.sb_flags |= SB_SEL;
		break;

	case 0:
		if (so->so_oobmark || (so->so_state & SS_RCVATMARK)) {
			splx(s);
			return (1);
		}
		selrecord(p, &so->so_rcv.sb_sel);
		so->so_rcv.sb_flags |= SB_SEL;
		break;
	}
	splx(s);
	return (0);
}

#ifndef __ECOS
int
soo_stat(so, ub)
	register struct socket *so;
	register struct stat *ub;
{

	bzero((caddr_t)ub, sizeof (*ub));
	ub->st_mode = S_IFSOCK;
	return ((*so->so_proto->pr_usrreq)(so, PRU_SENSE,
	    (struct mbuf *)ub, (struct mbuf *)0, 
	    (struct mbuf *)0));
}
#endif

#ifdef __ECOS
int
soo_close(struct file *fp)
#else
/* ARGSUSED */
int
soo_close(fp, p)
	struct file *fp;
	struct proc *p;
#endif
{
	int error = 0;

	if (fp->f_data)
		error = soclose((struct socket *)fp->f_data);
	fp->f_data = 0;
	return (error);
}
