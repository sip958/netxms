/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2010 Victor Kirhenshtein
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
package org.netxms.ui.eclipse.console;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.Properties;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.window.Window;
import org.eclipse.rap.rwt.RWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.application.IWorkbenchConfigurer;
import org.eclipse.ui.application.IWorkbenchWindowConfigurer;
import org.eclipse.ui.application.WorkbenchAdvisor;
import org.eclipse.ui.application.WorkbenchWindowAdvisor;
import org.eclipse.ui.model.ContributionComparator;
import org.eclipse.ui.model.IContributionService;
import org.netxms.api.client.Session;
import org.netxms.client.NXCSession;
import org.netxms.ui.eclipse.console.api.LoginForm;
import org.netxms.ui.eclipse.console.dialogs.PasswordExpiredDialog;
import org.netxms.ui.eclipse.jobs.LoginJob;
import org.netxms.ui.eclipse.shared.ConsoleSharedData;

/**
 * Workbench advisor for NetXMS console application
 */
public class ApplicationWorkbenchAdvisor extends WorkbenchAdvisor
{
   private static final String PERSPECTIVE_ID = "org.netxms.ui.eclipse.console.DefaultPerspective"; //$NON-NLS-1$
   
	/* (non-Javadoc)
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#createWorkbenchWindowAdvisor(org.eclipse.ui.application.IWorkbenchWindowConfigurer)
	 */
	public WorkbenchWindowAdvisor createWorkbenchWindowAdvisor(IWorkbenchWindowConfigurer configurer)
	{
		return new ApplicationWorkbenchWindowAdvisor(configurer);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#getInitialWindowPerspectiveId()
	 */
	public String getInitialWindowPerspectiveId()
	{
		String p = BrandingManager.getInstance().getDefaultPerspective();
		return (p != null) ? p : PERSPECTIVE_ID;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#initialize(org.eclipse.ui.application.IWorkbenchConfigurer)
	 */
	@Override
	public void initialize(IWorkbenchConfigurer configurer)
	{
		super.initialize(configurer);
		BrandingManager.create();
		configurer.setSaveAndRestore(true);
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#openWindows()
	 */
	@Override
	public boolean openWindows()
	{
		doLogin();
		return super.openWindows();
	}

	/* (non-Javadoc)
	 * @see org.eclipse.ui.application.WorkbenchAdvisor#getComparatorFor(java.lang.String)
	 */
	@Override
	public ContributionComparator getComparatorFor(String contributionType)
	{
		if (contributionType.equals(IContributionService.TYPE_PROPERTY))
			return new ExtendedContributionComparator();
		return super.getComparatorFor(contributionType);
	}

	/**
	 * Connect to NetXMS server
	 * 
	 * @param server
	 * @param login
	 * @param password
	 * @return
	 */
	private boolean connectToServer(String server, String login, String password)
	{
		boolean success = false;
		try
		{
			LoginJob job = new LoginJob(Display.getCurrent(), server, login, false, false);
			job.setPassword(password);
			ProgressMonitorDialog pd = new ProgressMonitorDialog(null);
			pd.run(false, false, job);
			success = true;
		}
		catch(InvocationTargetException e)
		{
			MessageDialog.openError(null, Messages.get().ApplicationWorkbenchWindowAdvisor_ConnectionError, e.getCause().getLocalizedMessage());
		}
		catch(Exception e)
		{
			e.printStackTrace();
			MessageDialog.openError(null, Messages.get().ApplicationWorkbenchWindowAdvisor_Exception, e.toString());
		}
		return success;
	}

	/**
	 * Hex digits
	 */
	final private static char[] HEX_DIGITS = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
	
	/**
	 * Convert byte array to hex string
	 * 
	 * @param bytes
	 * @return
	 */
	private static String bytesToHexString(byte[] bytes)
	{
		char[] hexChars = new char[bytes.length * 2];
		int v;
		for(int j = 0; j < bytes.length; j++)
		{
			v = bytes[j] & 0xFF;
			hexChars[j * 2] = HEX_DIGITS[v >>> 4];
			hexChars[j * 2 + 1] = HEX_DIGITS[v & 0x0F];
		}
		return new String(hexChars);
	}
	
	/**
	 * Show login dialog and perform login
	 */
	private void doLogin()
	{
		boolean success = false;
		
		final Properties properties = new AppPropertiesLoader().load();
		
		String password = "";
		boolean autoLogin = (RWT.getRequest().getParameter("auto") != null); //$NON-NLS-1$
		
		String ssoTicket = RWT.getRequest().getParameter("ticket");
		if (ssoTicket != null) 
		{
			autoLogin = true;
			String server = RWT.getRequest().getParameter("server"); //$NON-NLS-1$
			if (server == null)
				server = properties.getProperty("server", "127.0.0.1"); //$NON-NLS-1$ //$NON-NLS-2$
			success = connectToServer(server, null, ssoTicket);
		}
		else if (autoLogin) 
		{
			String server = RWT.getRequest().getParameter("server"); //$NON-NLS-1$
			if (server == null)
				server = properties.getProperty("server", "127.0.0.1"); //$NON-NLS-1$ //$NON-NLS-2$
			String login = RWT.getRequest().getParameter("login"); //$NON-NLS-1$
			if (login == null)
				login = "guest";
			password = RWT.getRequest().getParameter("password"); //$NON-NLS-1$
			if (password == null)
				password = "";
			success = connectToServer(server, login, password);
		}
		
		if (!autoLogin || !success)
		{
			Window loginDialog;
			do
			{
				loginDialog = BrandingManager.getInstance().getLoginForm(null, properties);
				if (loginDialog.open() != Window.OK)
					continue;
				password = ((LoginForm)loginDialog).getPassword();
				
				success = connectToServer(properties.getProperty("server", "127.0.0.1"),  //$NON-NLS-1$ //$NON-NLS-2$ 
				                          ((LoginForm)loginDialog).getLogin(),
				                          password);
			} while(!success);
		}

		if (success)
		{
			final Session session = (Session)RWT.getUISession().getAttribute(ConsoleSharedData.ATTRIBUTE_SESSION);
			
			try
			{
				RWT.getSettingStore().loadById(session.getUserName() + "@" + bytesToHexString(session.getServerId()));
			}
			catch(IOException e)
			{
			}
			
			// Suggest user to change password if it is expired
			if (session.isPasswordExpired())
			{
				final PasswordExpiredDialog dlg = new PasswordExpiredDialog(null);
				if (dlg.open() == Window.OK)
				{
					final String currentPassword = password;
					final Display display = Display.getCurrent();
					IRunnableWithProgress job = new IRunnableWithProgress() {
						@Override
						public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException
						{
							try
							{
								NXCSession session = (NXCSession)RWT.getUISession(display).getAttribute(ConsoleSharedData.ATTRIBUTE_SESSION);
								session.setUserPassword(session.getUserId(), dlg.getPassword(), currentPassword);
							}
							catch(Exception e)
							{
								throw new InvocationTargetException(e);
							}
							finally
							{
								monitor.done();
							}
						}
					};
					try
					{
						ProgressMonitorDialog pd = new ProgressMonitorDialog(null);
						pd.run(false, false, job);
						MessageDialog.openInformation(null, Messages.get().ApplicationWorkbenchWindowAdvisor_Information, Messages.get().ApplicationWorkbenchWindowAdvisor_PasswordChanged);
					}
					catch(InvocationTargetException e)
					{
						MessageDialog.openError(null, Messages.get().ApplicationWorkbenchWindowAdvisor_Error, Messages.get().ApplicationWorkbenchWindowAdvisor_CannotChangePswd + e.getCause().getLocalizedMessage());
					}
					catch(InterruptedException e)
					{
						MessageDialog.openError(null, Messages.get().ApplicationWorkbenchWindowAdvisor_Exception, e.toString());
					}
				}
			}
		}
	}
}
