/* automatically generated from pager-dialog.glade */
#ifdef __SUNPRO_C
#pragma align 4 (pager_dialog_ui)
#endif
#ifdef __GNUC__
static const char pager_dialog_ui[] __attribute__ ((__aligned__ (4))) =
#else
static const char pager_dialog_ui[] =
#endif
{
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><interface><requires lib=\"gt"
  "k+\" version=\"3.20\"/><requires lib=\"libxfce4ui-2\" version=\"4.12\"/"
  "><object class=\"GtkListStore\" id=\"appearance-model\"><columns><colum"
  "n type=\"gchararray\"/><column type=\"gint\"/></columns><data><row><col"
  " id=\"0\" translatable=\"yes\">Buttons</col><col id=\"1\">0</col></row>"
  "<row><col id=\"0\" translatable=\"yes\">Miniature View</col><col id=\"1"
  "\">1</col></row></data></object><object class=\"GtkImage\" id=\"image1\""
  "><property name=\"visible\">True</property><property name=\"can_focus\""
  ">False</property><property name=\"icon_name\">org.xfce.workspaces</prop"
  "erty></object><object class=\"GtkImage\" id=\"image2\"><property name=\""
  "visible\">True</property><property name=\"can_focus\">False</property><"
  "property name=\"icon_name\">help-browser</property></object><object cla"
  "ss=\"GtkImage\" id=\"image3\"><property name=\"visible\">True</property"
  "><property name=\"can_focus\">False</property><property name=\"icon_nam"
  "e\">window-close-symbolic</property></object><object class=\"GtkAdjustm"
  "ent\" id=\"rows\"><property name=\"lower\">1</property><property name=\""
  "upper\">10</property><property name=\"value\">1</property><property nam"
  "e=\"step_increment\">1</property><property name=\"page_increment\">10</"
  "property></object><object class=\"XfceTitledDialog\" id=\"dialog\"><pro"
  "perty name=\"can_focus\">False</property><property name=\"title\" trans"
  "latable=\"yes\">Workspace Switcher</property><property name=\"icon_name"
  "\">org.xfce.panel.pager</property><property name=\"type_hint\">normal</"
  "property><child internal-child=\"vbox\"><object class=\"GtkBox\" id=\"d"
  "ialog-vbox2\"><property name=\"visible\">True</property><property name="
  "\"can_focus\">False</property><property name=\"orientation\">vertical</"
  "property><property name=\"spacing\">2</property><child internal-child=\""
  "action_area\"><object class=\"GtkButtonBox\" id=\"dialog-action_area2\""
  "><property name=\"visible\">True</property><property name=\"can_focus\""
  ">False</property><property name=\"layout_style\">end</property><child><"
  "object class=\"GtkButton\" id=\"help-button\"><property name=\"label\" "
  "translatable=\"yes\">_Help</property><property name=\"visible\">True</p"
  "roperty><property name=\"can_focus\">True</property><property name=\"re"
  "ceives_default\">True</property><property name=\"image\">image2</proper"
  "ty><property name=\"use_underline\">True</property></object><packing><p"
  "roperty name=\"expand\">False</property><property name=\"fill\">False</"
  "property><property name=\"position\">0</property><property name=\"secon"
  "dary\">True</property></packing></child><child><object class=\"GtkButto"
  "n\" id=\"close-button\"><property name=\"label\" translatable=\"yes\">_"
  "Close</property><property name=\"visible\">True</property><property nam"
  "e=\"can_focus\">True</property><property name=\"receives_default\">True"
  "</property><property name=\"image\">image3</property><property name=\"u"
  "se_underline\">True</property></object><packing><property name=\"expand"
  "\">False</property><property name=\"fill\">False</property><property na"
  "me=\"position\">2</property></packing></child><child type=\"center\"><o"
  "bject class=\"GtkButton\" id=\"settings-button\"><property name=\"label"
  "\" translatable=\"yes\">Workspace _Settings...</property><property name"
  "=\"visible\">True</property><property name=\"can_focus\">True</property"
  "><property name=\"receives_default\">True</property><property name=\"ha"
  "lign\">start</property><property name=\"image\">image1</property><prope"
  "rty name=\"use_underline\">True</property></object><packing><property n"
  "ame=\"expand\">False</property><property name=\"fill\">True</property><"
  "property name=\"position\">1</property><property name=\"secondary\">Tru"
  "e</property></packing></child></object><packing><property name=\"expand"
  "\">False</property><property name=\"fill\">False</property><property na"
  "me=\"pack_type\">end</property><property name=\"position\">0</property>"
  "</packing></child><child><object class=\"GtkGrid\"><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">False</property><pro"
  "perty name=\"border_width\">6</property><property name=\"row_spacing\">"
  "6</property><property name=\"column_spacing\">12</property><child><obje"
  "ct class=\"GtkLabel\" id=\"label4\"><property name=\"visible\">True</pr"
  "operty><property name=\"can_focus\">False</property><property name=\"ma"
  "rgin_left\">12</property><property name=\"label\" translatable=\"yes\">"
  "Number of _rows:</property><property name=\"use_underline\">True</prope"
  "rty><property name=\"xalign\">0</property></object><packing><property n"
  "ame=\"left_attach\">0</property><property name=\"top_attach\">1</proper"
  "ty></packing></child><child><object class=\"GtkSpinButton\" id=\"spin1\""
  "><property name=\"visible\">True</property><property name=\"can_focus\""
  ">True</property><property name=\"halign\">end</property><property name="
  "\"adjustment\">rows</property><property name=\"snap_to_ticks\">True</pr"
  "operty><property name=\"numeric\">True</property><property name=\"updat"
  "e_policy\">if-valid</property></object><packing><property name=\"left_a"
  "ttach\">1</property><property name=\"top_attach\">1</property></packing"
  "></child><child><object class=\"GtkSwitch\" id=\"numbering\"><property "
  "name=\"visible\">True</property><property name=\"can_focus\">True</prop"
  "erty><property name=\"halign\">end</property><property name=\"valign\">"
  "center</property></object><packing><property name=\"left_attach\">1</pr"
  "operty><property name=\"top_attach\">2</property></packing></child><chi"
  "ld><object class=\"GtkSwitch\" id=\"workspace-scrolling\"><property nam"
  "e=\"visible\">True</property><property name=\"can_focus\">True</propert"
  "y><property name=\"halign\">end</property><property name=\"valign\">cen"
  "ter</property></object><packing><property name=\"left_attach\">1</prope"
  "rty><property name=\"top_attach\">3</property></packing></child><child>"
  "<object class=\"GtkSwitch\" id=\"wrap-workspaces\"><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">True</property><prop"
  "erty name=\"halign\">end</property><property name=\"valign\">center</pr"
  "operty><property name=\"sensitive\" bind-source=\"workspace-scrolling\""
  "\n                          bind-property=\"active\" bind-flags=\"G_BIN"
  "DING_SYNC_CREATE\"/></object><packing><property name=\"left_attach\">1<"
  "/property><property name=\"top_attach\">4</property></packing></child><"
  "child><object class=\"GtkLabel\" id=\"numbering-label\"><property name="
  "\"visible\">True</property><property name=\"can_focus\">False</property"
  "><property name=\"margin_left\">12</property><property name=\"label\" t"
  "ranslatable=\"yes\">Show workspace number</property><property name=\"xa"
  "lign\">0</property></object><packing><property name=\"left_attach\">0</"
  "property><property name=\"top_attach\">2</property></packing></child><c"
  "hild><object class=\"GtkLabel\" id=\"workspace-scrolling-label\"><prope"
  "rty name=\"visible\">True</property><property name=\"can_focus\">False<"
  "/property><property name=\"margin_left\">12</property><property name=\""
  "label\" translatable=\"yes\">Switch workspaces using the mouse wheel</p"
  "roperty><property name=\"xalign\">0</property></object><packing><proper"
  "ty name=\"left_attach\">0</property><property name=\"top_attach\">3</pr"
  "operty></packing></child><child><object class=\"GtkLabel\" id=\"wrap-wo"
  "rkspaces-label\"><property name=\"visible\">True</property><property na"
  "me=\"can_focus\">False</property><property name=\"margin_left\">12</pro"
  "perty><property name=\"label\" translatable=\"yes\">Wrap around workspa"
  "ces</property><property name=\"xalign\">0</property><property name=\"se"
  "nsitive\" bind-source=\"workspace-scrolling\"\n                        "
  "  bind-property=\"active\" bind-flags=\"G_BINDING_SYNC_CREATE\"/></obje"
  "ct><packing><property name=\"left_attach\">0</property><property name=\""
  "top_attach\">4</property></packing></child><child><object class=\"GtkBo"
  "x\"><property name=\"visible\">True</property><property name=\"can_focu"
  "s\">False</property><property name=\"spacing\">12</property><child><obj"
  "ect class=\"GtkLabel\" id=\"label5\"><property name=\"visible\">True</p"
  "roperty><property name=\"can_focus\">False</property><property name=\"l"
  "abel\" translatable=\"yes\">&lt;b&gt;Appearance&lt;/b&gt;</property><pr"
  "operty name=\"use_markup\">True</property><property name=\"xalign\">0</"
  "property></object><packing><property name=\"expand\">False</property><p"
  "roperty name=\"fill\">True</property><property name=\"position\">0</pro"
  "perty></packing></child><child><object class=\"GtkComboBox\" id=\"appea"
  "rance\"><property name=\"visible\">True</property><property name=\"can_"
  "focus\">False</property><property name=\"halign\">end</property><proper"
  "ty name=\"hexpand\">True</property><property name=\"model\">appearance-"
  "model</property><property name=\"active\">0</property><property name=\""
  "id_column\">1</property><child><object class=\"GtkCellRendererText\" id"
  "=\"renderer\"/><attributes><attribute name=\"text\">0</attribute></attr"
  "ibutes></child></object><packing><property name=\"expand\">False</prope"
  "rty><property name=\"fill\">True</property><property name=\"position\">"
  "1</property></packing></child></object><packing><property name=\"left_a"
  "ttach\">0</property><property name=\"top_attach\">0</property><property"
  " name=\"width\">2</property></packing></child></object><packing><proper"
  "ty name=\"expand\">True</property><property name=\"fill\">True</propert"
  "y><property name=\"position\">1</property></packing></child></object></"
  "child><action-widgets><action-widget response=\"0\">help-button</action"
  "-widget><action-widget response=\"0\">close-button</action-widget></act"
  "ion-widgets></object></interface>"
};

static const unsigned pager_dialog_ui_length = 9091u;

