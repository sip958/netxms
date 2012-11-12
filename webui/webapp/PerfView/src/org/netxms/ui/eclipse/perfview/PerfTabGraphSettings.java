/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2012 Victor Kirhenshtein
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
package org.netxms.ui.eclipse.perfview;

import java.io.StringWriter;
import java.io.Writer;

import org.netxms.client.datacollection.GraphItemStyle;
import org.netxms.client.datacollection.PerfTabDci;
import org.simpleframework.xml.Element;
import org.simpleframework.xml.Root;
import org.simpleframework.xml.Serializer;
import org.simpleframework.xml.core.Persister;

/**
 * Settings for performance tab graph
 */
@Root(name="config", strict=false)
public class PerfTabGraphSettings
{
	@Element(required=false)
	private boolean enabled = false;
	
	@Element(required=false)
	private int type = GraphItemStyle.LINE;
	
	@Element(required=false)
	private String color = "0x00C000";
	
	@Element(required=false)
	private String title = "";
	
	@Element(required=false)
	private String name = "";
	
	@Element(required=false)
	private boolean showThresholds = false;
	
	@Element(required=false)
	private long parentDciId = 0;

	@Element(required=false)
	private String parentDciName = null;
	
	private PerfTabDci runtimeDciInfo = null;

	/**
	 * Create performance tab graph settings object from XML document
	 * 
	 * @param xml XML document
	 * @return deserialized object
	 * @throws Exception if the object cannot be fully deserialized
	 */
	public static PerfTabGraphSettings createFromXml(final String xml) throws Exception
	{
		Serializer serializer = new Persister();
		return serializer.read(PerfTabGraphSettings.class, xml);
	}
	
	/**
	 * Create XML document from object
	 * 
	 * @return XML document
	 * @throws Exception if the schema for the object is not valid
	 */
	public String createXml() throws Exception
	{
		Serializer serializer = new Persister();
		Writer writer = new StringWriter();
		serializer.write(this, writer);
		return writer.toString();
	}

	/**
	 * @return the enabled
	 */
	public boolean isEnabled()
	{
		return enabled;
	}

	/**
	 * @return the type
	 */
	public int getType()
	{
		return type;
	}

	/**
	 * @return the color
	 */
	public String getColor()
	{
		return color;
	}

	/**
	 * @return the color
	 */
	public int getColorAsInt()
	{
		if (color.startsWith("0x"))
			return Integer.parseInt(color.substring(2), 16);
		return Integer.parseInt(color, 10);
	}

	/**
	 * @return the title
	 */
	public String getTitle()
	{
		return title;
	}

	/**
	 * @param enabled the enabled to set
	 */
	public void setEnabled(boolean enabled)
	{
		this.enabled = enabled;
	}

	/**
	 * @param type the type to set
	 */
	public void setType(int type)
	{
		this.type = type;
	}

	/**
	 * @param color the color to set
	 */
	public void setColor(final String color)
	{
		this.color = color;
	}

	/**
	 * @param color the color to set
	 */
	public void setColor(final int color)
	{
		this.color = Integer.toString(color);
	}

	/**
	 * @param title the title to set
	 */
	public void setTitle(String title)
	{
		this.title = title;
	}

	/**
	 * @return the showThresholds
	 */
	public boolean isShowThresholds()
	{
		return showThresholds;
	}

	/**
	 * @param showThresholds the showThresholds to set
	 */
	public void setShowThresholds(boolean showThresholds)
	{
		this.showThresholds = showThresholds;
	}

	/**
	 * @return the parentDciId
	 */
	public final long getParentDciId()
	{
		return parentDciId;
	}

	/**
	 * @param parentDciId the parentDciId to set
	 */
	public final void setParentDciId(long parentDciId)
	{
		this.parentDciId = parentDciId;
	}

	/**
	 * @return the parentDciName
	 */
	public final String getParentDciName()
	{
		return parentDciName;
	}

	/**
	 * @param parentDciName the parentDciName to set
	 */
	public final void setParentDciName(String parentDciName)
	{
		this.parentDciName = parentDciName;
	}

	/**
	 * @return the runtimeDciInfo
	 */
	public final PerfTabDci getRuntimeDciInfo()
	{
		return runtimeDciInfo;
	}

	/**
	 * @param runtimeDciInfo the runtimeDciInfo to set
	 */
	public final void setRuntimeDciInfo(PerfTabDci runtimeDciInfo)
	{
		this.runtimeDciInfo = runtimeDciInfo;
	}

	/**
	 * @return the name
	 */
	public final String getName()
	{
		return name;
	}

	/**
	 * @param name the name to set
	 */
	public final void setName(String name)
	{
		this.name = name;
	}
}
