/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2011 Victor Kirhenshtein
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
package org.netxms.ui.eclipse.datacollection.propertypages;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.dialogs.PropertyPage;
import org.netxms.client.datacollection.ColumnDefinition;
import org.netxms.client.datacollection.DataCollectionTable;
import org.netxms.client.datacollection.Threshold;
import org.netxms.ui.eclipse.datacollection.Activator;
import org.netxms.ui.eclipse.datacollection.ThresholdLabelProvider;
import org.netxms.ui.eclipse.datacollection.dialogs.EditColumnDialog;
import org.netxms.ui.eclipse.datacollection.dialogs.EditThresholdDialog;
import org.netxms.ui.eclipse.tools.WidgetHelper;
import org.netxms.ui.eclipse.widgets.LabeledText;

/**
 * "Columns" property page for table DCI
 */
public class TableColumns extends PropertyPage
{
	private static final String COLUMN_SETTINGS_PREFIX = "TableColumns.ColumnList";
	
	private DataCollectionTable dci;
	private List<ColumnDefinition> columns;
	private LabeledText instanceColumn;
	private TableViewer columnList;
	private Button addButton;
	private Button modifyButton;
	private Button deleteButton;

	/* (non-Javadoc)
	 * @see org.eclipse.jface.preference.PreferencePage#createContents(org.eclipse.swt.widgets.Composite)
	 */
	@Override
	protected Control createContents(Composite parent)
	{
		dci = (DataCollectionTable)getElement().getAdapter(DataCollectionTable.class);
		columns = new ArrayList<ColumnDefinition>();
		
		Composite dialogArea = new Composite(parent, SWT.NONE);
		
		GridLayout layout = new GridLayout();
		layout.verticalSpacing = WidgetHelper.OUTER_SPACING;
		layout.marginWidth = 0;
		layout.marginHeight = 0;
      dialogArea.setLayout(layout);

      instanceColumn = new LabeledText(dialogArea, SWT.NONE);
      instanceColumn.setLabel("Instance column");
      instanceColumn.setText(dci.getInstanceColumn());
      GridData gd = new GridData();
      gd.horizontalAlignment = SWT.FILL;
      gd.grabExcessHorizontalSpace = true;
      instanceColumn.setLayoutData(gd);
      
      Composite columnListArea = new Composite(dialogArea, SWT.NONE);
      gd = new GridData();
      gd.horizontalAlignment = SWT.FILL;
      gd.grabExcessHorizontalSpace = true;
      gd.verticalAlignment = SWT.FILL;
      gd.grabExcessVerticalSpace = true;
      gd.horizontalSpan = 2;
      columnListArea.setLayoutData(gd);
      layout = new GridLayout();
		layout.verticalSpacing = WidgetHelper.INNER_SPACING;
		layout.marginWidth = 0;
		layout.marginHeight = 0;
		layout.numColumns = 2;
      columnListArea.setLayout(layout);
	
      new Label(columnListArea, SWT.NONE).setText("Columns");
      
      columnList = new TableViewer(columnListArea, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION);
      gd = new GridData();
      gd.horizontalAlignment = SWT.FILL;
      gd.grabExcessHorizontalSpace = true;
      gd.verticalAlignment = SWT.FILL;
      gd.grabExcessVerticalSpace = true;
      gd.horizontalSpan = 2;
      columnList.getControl().setLayoutData(gd);
      setupColumnList();
      columnList.setInput(columns.toArray());
      
      Composite leftButtons = new Composite(columnListArea, SWT.NONE);
      gd = new GridData();
      gd.horizontalAlignment = SWT.LEFT;
      leftButtons.setLayoutData(gd);
      RowLayout buttonsLayout = new RowLayout(SWT.HORIZONTAL);
      buttonsLayout.marginBottom = 0;
      buttonsLayout.marginLeft = 0;
      buttonsLayout.marginRight = 0;
      buttonsLayout.marginTop = 0;
      buttonsLayout.spacing = WidgetHelper.OUTER_SPACING;
      buttonsLayout.fill = true;
      buttonsLayout.pack = false;
      leftButtons.setLayout(buttonsLayout);

      Composite buttons = new Composite(columnListArea, SWT.NONE);
      gd = new GridData();
      gd.horizontalAlignment = SWT.RIGHT;
      buttons.setLayoutData(gd);
      buttonsLayout = new RowLayout(SWT.HORIZONTAL);
      buttonsLayout.marginBottom = 0;
      buttonsLayout.marginLeft = 0;
      buttonsLayout.marginRight = 0;
      buttonsLayout.marginTop = 0;
      buttonsLayout.spacing = WidgetHelper.OUTER_SPACING;
      buttonsLayout.fill = true;
      buttonsLayout.pack = false;
      buttons.setLayout(buttonsLayout);
      
      addButton = new Button(buttons, SWT.PUSH);
      addButton.setText("&Add...");
      RowData rd = new RowData();
      rd.width = WidgetHelper.BUTTON_WIDTH_HINT;
      addButton.setLayoutData(rd);
      addButton.addSelectionListener(new SelectionListener() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e)
			{
				widgetSelected(e);
			}
			
			@Override
			public void widgetSelected(SelectionEvent e)
			{
				addColumn();
			}
		});
      
      modifyButton = new Button(buttons, SWT.PUSH);
      modifyButton.setText("&Edit...");
      rd = new RowData();
      rd.width = WidgetHelper.BUTTON_WIDTH_HINT;
      modifyButton.setLayoutData(rd);
      modifyButton.setEnabled(false);
      modifyButton.addSelectionListener(new SelectionListener() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e)
			{
				widgetSelected(e);
			}

			@Override
			public void widgetSelected(SelectionEvent e)
			{
				editColumn();
			}
      });
      
      deleteButton = new Button(buttons, SWT.PUSH);
      deleteButton.setText("&Delete");
      rd = new RowData();
      rd.width = WidgetHelper.BUTTON_WIDTH_HINT;
      deleteButton.setLayoutData(rd);
      deleteButton.setEnabled(false);
      deleteButton.addSelectionListener(new SelectionListener() {
			@Override
			public void widgetDefaultSelected(SelectionEvent e)
			{
				widgetSelected(e);
			}

			@Override
			public void widgetSelected(SelectionEvent e)
			{
				deleteColumns();
			}
      });
      
      /*** Selection change listener for thresholds list ***/
      columnList.addSelectionChangedListener(new ISelectionChangedListener() {
			@Override
			public void selectionChanged(SelectionChangedEvent event)
			{
				IStructuredSelection selection = (IStructuredSelection)event.getSelection();
				modifyButton.setEnabled(selection.size() == 1);
				deleteButton.setEnabled(selection.size() > 0);
			}
      });
      
      columnList.addDoubleClickListener(new IDoubleClickListener() {
			@Override
			public void doubleClick(DoubleClickEvent event)
			{
				editColumn();
			}
      });

      return dialogArea;
	}

	/**
	 * Setup threshold list control
	 */
	private void setupColumnList()
	{
		Table table = columnList.getTable();
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		
		TableColumn column = new TableColumn(table, SWT.LEFT);
		column.setText("Expression");
		column.setWidth(200);
		
		column = new TableColumn(table, SWT.LEFT);
		column.setText("Event");
		column.setWidth(150);
		
		WidgetHelper.restoreColumnSettings(table, Activator.getDefault().getDialogSettings(), COLUMN_SETTINGS_PREFIX);
		
		columnList.setContentProvider(new ArrayContentProvider());
		columnList.setLabelProvider(new ThresholdLabelProvider());
	}

	/**
	 * Delete selected columns
	 */
	@SuppressWarnings("unchecked")
	private void deleteColumns()
	{
		final IStructuredSelection selection = (IStructuredSelection)columnList.getSelection();
		if (!selection.isEmpty())
		{
			Iterator<Threshold> it = selection.iterator();
			while(it.hasNext())
			{
				columns.remove(it.next());
			}
	      columnList.setInput(columns.toArray());
		}
	}
	
	/**
	 * Edit selected threshold
	 */
	private void editColumn()
	{
		final IStructuredSelection selection = (IStructuredSelection)columnList.getSelection();
		if (selection.size() == 1)
		{
			final ColumnDefinition column = (ColumnDefinition)selection.getFirstElement();
			EditColumnDialog dlg = new EditColumnDialog(getShell(), column);
			if (dlg.open() == Window.OK)
			{
				columnList.update(column, null);
			}
		}
	}
	
	/**	
	 * Add new threshold
	 */
	private void addColumn()
	{
		final InputDialog idlg = new InputDialog(getShell(), "New column definition", "Column name", "", new IInputValidator() {
			@Override
			public String isValid(String newText)
			{
				if (newText.trim().isEmpty())
					return "Please enter non-empty column name";
				return null;
			}
		});
		if (idlg.open() == Window.OK)
		{
			final ColumnDefinition column = new ColumnDefinition(idlg.getValue());
			final EditColumnDialog dlg = new EditColumnDialog(getShell(), column);
			if (dlg.open() == Window.OK)
			{
				columns.add(column);
		      columnList.setInput(columns.toArray());
		      columnList.setSelection(new StructuredSelection(column));
			}
		}
	}
}
