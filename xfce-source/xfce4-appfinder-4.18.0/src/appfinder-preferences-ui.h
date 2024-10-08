/* automatically generated from appfinder-preferences.glade */
#ifdef __SUNPRO_C
#pragma align 4 (appfinder_preferences_ui)
#endif
#ifdef __GNUC__
static const char appfinder_preferences_ui[] __attribute__ ((__aligned__ (4))) =
#else
static const char appfinder_preferences_ui[] =
#endif
{
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><interface><requires lib=\"gt"
  "k+\" version=\"3.22\"/><requires lib=\"libxfce4ui-2\" version=\"4.14\"/"
  "><object class=\"GtkListStore\" id=\"action-types\"><columns><column ty"
  "pe=\"gchararray\"/></columns><data><row><col id=\"0\" translatable=\"ye"
  "s\">Prefix</col></row><row><col id=\"0\" translatable=\"yes\">Regular E"
  "xpression</col></row></data></object><object class=\"GtkListStore\" id="
  "\"actions-store\"><columns><column type=\"gchararray\"/><column type=\""
  "gint\"/></columns></object><object class=\"GtkListStore\" id=\"icon-siz"
  "es\"><columns><column type=\"gchararray\"/></columns><data><row><col id"
  "=\"0\" translatable=\"yes\">Very Small</col></row><row><col id=\"0\" tr"
  "anslatable=\"yes\">Smaller</col></row><row><col id=\"0\" translatable=\""
  "yes\">Small</col></row><row><col id=\"0\" translatable=\"yes\">Normal</"
  "col></row><row><col id=\"0\" translatable=\"yes\">Large</col></row><row"
  "><col id=\"0\" translatable=\"yes\">Larger</col></row><row><col id=\"0\""
  " translatable=\"yes\">Very Large</col></row></data></object><object cla"
  "ss=\"GtkImage\" id=\"image3\"><property name=\"visible\">True</property"
  "><property name=\"can_focus\">False</property><property name=\"icon_nam"
  "e\">edit-clear-symbolic</property></object><object class=\"GtkImage\" i"
  "d=\"image4\"><property name=\"visible\">True</property><property name=\""
  "can_focus\">False</property><property name=\"icon_name\">window-close-s"
  "ymbolic</property></object><object class=\"GtkImage\" id=\"image5\"><pr"
  "operty name=\"visible\">True</property><property name=\"can_focus\">Fal"
  "se</property><property name=\"icon_name\">help-browser-symbolic</proper"
  "ty></object><object class=\"XfceTitledDialog\" id=\"dialog\"><property "
  "name=\"can_focus\">False</property><property name=\"title\" translatabl"
  "e=\"yes\">Application Finder</property><property name=\"default_width\""
  ">385</property><property name=\"default_height\">425</property><propert"
  "y name=\"icon_name\">gtk-preferences</property><property name=\"type_hi"
  "nt\">dialog</property><child internal-child=\"vbox\"><object class=\"Gt"
  "kVBox\" id=\"dialog-vbox1\"><property name=\"visible\">True</property><"
  "property name=\"can_focus\">False</property><property name=\"spacing\">"
  "2</property><child internal-child=\"action_area\"><object class=\"GtkHB"
  "uttonBox\" id=\"dialog-action_area1\"><property name=\"visible\">True</"
  "property><property name=\"can_focus\">False</property><property name=\""
  "layout_style\">end</property><child><object class=\"GtkButton\" id=\"bu"
  "tton-close\"><property name=\"label\" translatable=\"yes\">_Close</prop"
  "erty><property name=\"use_action_appearance\">False</property><property"
  " name=\"visible\">True</property><property name=\"can_focus\">True</pro"
  "perty><property name=\"receives_default\">True</property><property name"
  "=\"image\">image4</property><property name=\"use_underline\">True</prop"
  "erty></object><packing><property name=\"expand\">False</property><prope"
  "rty name=\"fill\">False</property><property name=\"position\">0</proper"
  "ty></packing></child><child><object class=\"GtkButton\" id=\"button-hel"
  "p\"><property name=\"label\" translatable=\"yes\">_Help</property><prop"
  "erty name=\"use_action_appearance\">False</property><property name=\"vi"
  "sible\">True</property><property name=\"can_focus\">True</property><pro"
  "perty name=\"receives_default\">False</property><property name=\"image\""
  ">image5</property><property name=\"use_underline\">True</property></obj"
  "ect><packing><property name=\"expand\">False</property><property name=\""
  "fill\">False</property><property name=\"position\">0</property><propert"
  "y name=\"secondary\">True</property></packing></child></object><packing"
  "><property name=\"expand\">False</property><property name=\"fill\">True"
  "</property><property name=\"pack_type\">end</property><property name=\""
  "position\">0</property></packing></child><child><object class=\"GtkNote"
  "book\" id=\"notebook1\"><property name=\"visible\">True</property><prop"
  "erty name=\"can_focus\">True</property><property name=\"border_width\">"
  "6</property><child><object class=\"GtkVBox\" id=\"vbox3\"><property nam"
  "e=\"visible\">True</property><property name=\"can_focus\">False</proper"
  "ty><property name=\"border_width\">12</property><property name=\"spacin"
  "g\">18</property><child><object class=\"GtkFrame\" id=\"frame1\"><prope"
  "rty name=\"visible\">True</property><property name=\"can_focus\">False<"
  "/property><property name=\"label_xalign\">0</property><property name=\""
  "shadow_type\">none</property><child><object class=\"GtkAlignment\" id=\""
  "alignment2\"><property name=\"visible\">True</property><property name=\""
  "can_focus\">False</property><property name=\"top_padding\">6</property>"
  "<property name=\"left_padding\">12</property><child><object class=\"Gtk"
  "VBox\" id=\"vbox4\"><property name=\"visible\">True</property><property"
  " name=\"can_focus\">False</property><property name=\"spacing\">6</prope"
  "rty><child><object class=\"GtkCheckButton\" id=\"remember-category\"><p"
  "roperty name=\"label\" translatable=\"yes\">Remember last _selected cat"
  "egory</property><property name=\"use_action_appearance\">False</propert"
  "y><property name=\"visible\">True</property><property name=\"can_focus\""
  ">True</property><property name=\"receives_default\">False</property><pr"
  "operty name=\"use_underline\">True</property><property name=\"draw_indi"
  "cator\">True</property></object><packing><property name=\"expand\">True"
  "</property><property name=\"fill\">True</property><property name=\"posi"
  "tion\">0</property></packing></child><child><object class=\"GtkCheckBut"
  "ton\" id=\"always-center\"><property name=\"label\" translatable=\"yes\""
  ">Always c_enter the window</property><property name=\"use_action_appear"
  "ance\">False</property><property name=\"visible\">True</property><prope"
  "rty name=\"can_focus\">True</property><property name=\"receives_default"
  "\">False</property><property name=\"tooltip_text\" translatable=\"yes\""
  ">Center the window on startup.</property><property name=\"use_underline"
  "\">True</property><property name=\"draw_indicator\">True</property></ob"
  "ject><packing><property name=\"expand\">True</property><property name=\""
  "fill\">True</property><property name=\"position\">1</property></packing"
  "></child><child><object class=\"GtkCheckButton\" id=\"enable-service\">"
  "<property name=\"label\" translatable=\"yes\">Keep running _instance in"
  " the background</property><property name=\"use_action_appearance\">Fals"
  "e</property><property name=\"visible\">True</property><property name=\""
  "can_focus\">True</property><property name=\"receives_default\">False</p"
  "roperty><property name=\"tooltip_text\" translatable=\"yes\">Instead of"
  " quitting the application when the last window is closed, keep a runnin"
  "g instance to speed up opening new windows. You might want to disable t"
  "his to reduce memory usage.</property><property name=\"use_underline\">"
  "True</property><property name=\"active\">True</property><property name="
  "\"draw_indicator\">True</property></object><packing><property name=\"ex"
  "pand\">True</property><property name=\"fill\">True</property><property "
  "name=\"position\">2</property></packing></child><child><object class=\""
  "GtkCheckButton\" id=\"single-window\"><property name=\"label\" translat"
  "able=\"yes\">Single window</property><property name=\"use_action_appear"
  "ance\">False</property><property name=\"visible\">True</property><prope"
  "rty name=\"can_focus\">True</property><property name=\"receives_default"
  "\">False</property><property name=\"tooltip_text\" translatable=\"yes\""
  ">When an instance is running in the background, only open one window at"
  " a time.</property><property name=\"use_underline\">True</property><pro"
  "perty name=\"active\">True</property><property name=\"draw_indicator\">"
  "True</property></object><packing><property name=\"expand\">True</proper"
  "ty><property name=\"fill\">True</property><property name=\"position\">3"
  "</property></packing></child><child><object class=\"GtkCheckButton\" id"
  "=\"sort-by-frecency\"><property name=\"label\" translatable=\"yes\">Sor"
  "t recently used items first</property><property name=\"use_action_appea"
  "rance\">False</property><property name=\"visible\">True</property><prop"
  "erty name=\"can_focus\">True</property><property name=\"receives_defaul"
  "t\">False</property><property name=\"tooltip_text\" translatable=\"yes\""
  ">Order items, such that items that are most recently used are always on"
  " the top.</property><property name=\"use_underline\">True</property><pr"
  "operty name=\"draw_indicator\">True</property></object><packing><proper"
  "ty name=\"expand\">True</property><property name=\"fill\">True</propert"
  "y><property name=\"position\">4</property></packing></child></object></"
  "child></object></child><child type=\"label\"><object class=\"GtkLabel\""
  " id=\"label6\"><property name=\"visible\">True</property><property name"
  "=\"can_focus\">False</property><property name=\"label\" translatable=\""
  "yes\">Behaviour</property><attributes><attribute name=\"weight\" value="
  "\"bold\"/></attributes></object></child></object><packing><property nam"
  "e=\"expand\">False</property><property name=\"fill\">True</property><pr"
  "operty name=\"position\">0</property></packing></child><child><object c"
  "lass=\"GtkFrame\" id=\"frame3\"><property name=\"visible\">True</proper"
  "ty><property name=\"can_focus\">False</property><property name=\"label_"
  "xalign\">0</property><property name=\"shadow_type\">none</property><chi"
  "ld><object class=\"GtkAlignment\" id=\"alignment5\"><property name=\"vi"
  "sible\">True</property><property name=\"can_focus\">False</property><pr"
  "operty name=\"top_padding\">6</property><property name=\"left_padding\""
  ">12</property><child><object class=\"GtkTable\" id=\"table2\"><property"
  " name=\"visible\">True</property><property name=\"can_focus\">False</pr"
  "operty><property name=\"n_rows\">4</property><property name=\"n_columns"
  "\">2</property><property name=\"column_spacing\">12</property><property"
  " name=\"row_spacing\">6</property><child><object class=\"GtkCheckButton"
  "\" id=\"icon-view\"><property name=\"label\" translatable=\"yes\">_View"
  " items as icons</property><property name=\"use_action_appearance\">Fals"
  "e</property><property name=\"visible\">True</property><property name=\""
  "can_focus\">True</property><property name=\"receives_default\">False</p"
  "roperty><property name=\"use_underline\">True</property><property name="
  "\"draw_indicator\">True</property></object><packing><property name=\"ri"
  "ght_attach\">2</property><property name=\"y_options\"/></packing></chil"
  "d><child><object class=\"GtkCheckButton\" id=\"text-beside-icons\"><pro"
  "perty name=\"label\" translatable=\"yes\">Text besi_de icons</property>"
  "<property name=\"use_action_appearance\">False</property><property name"
  "=\"visible\">True</property><property name=\"can_focus\">True</property"
  "><property name=\"receives_default\">False</property><property name=\"u"
  "se_underline\">True</property><property name=\"draw_indicator\">True</p"
  "roperty></object><packing><property name=\"right_attach\">2</property><"
  "property name=\"top_attach\">1</property><property name=\"y_options\"/>"
  "</packing></child><child><object class=\"GtkLabel\" id=\"label9\"><prop"
  "erty name=\"visible\">True</property><property name=\"can_focus\">False"
  "</property><property name=\"label\" translatable=\"yes\">Ite_m icon siz"
  "e:</property><property name=\"use_underline\">True</property><property "
  "name=\"mnemonic_widget\">item-icon-size</property><property name=\"xali"
  "gn\">0</property></object><packing><property name=\"top_attach\">2</pro"
  "perty><property name=\"bottom_attach\">3</property><property name=\"x_o"
  "ptions\">GTK_FILL</property><property name=\"y_options\"/></packing></c"
  "hild><child><object class=\"GtkCheckButton\" id=\"hide-category-pane\">"
  "<property name=\"label\" translatable=\"yes\">Hide category pane</prope"
  "rty><property name=\"use_action_appearance\">False</property><property "
  "name=\"visible\">True</property><property name=\"can_focus\">True</prop"
  "erty><property name=\"receives_default\">False</property><property name"
  "=\"tooltip_text\" translatable=\"yes\">Hide category panel and show all"
  " applications.</property><property name=\"use_underline\">True</propert"
  "y><property name=\"active\">False</property><property name=\"draw_indic"
  "ator\">True</property></object><packing><property name=\"left_attach\">"
  "0</property><property name=\"right_attach\">2</property><property name="
  "\"top_attach\">3</property><property name=\"bottom_attach\">4</property"
  "><property name=\"y_options\"/></packing></child><child><object class=\""
  "GtkLabel\" id=\"label10\"><property name=\"visible\">True</property><pr"
  "operty name=\"can_focus\">False</property><property name=\"label\" tran"
  "slatable=\"yes\">Categ_ory icon size:</property><property name=\"use_un"
  "derline\">True</property><property name=\"mnemonic_widget\">category-ic"
  "on-size</property><property name=\"xalign\">0</property></object><packi"
  "ng><property name=\"top_attach\">4</property><property name=\"bottom_at"
  "tach\">5</property><property name=\"x_options\">GTK_FILL</property><pro"
  "perty name=\"y_options\"/></packing></child><child><object class=\"GtkC"
  "omboBox\" id=\"item-icon-size\"><property name=\"visible\">True</proper"
  "ty><property name=\"can_focus\">False</property><property name=\"model\""
  ">icon-sizes</property><child><object class=\"GtkCellRendererText\" id=\""
  "cellrenderertext4\"/><attributes><attribute name=\"text\">0</attribute>"
  "</attributes></child></object><packing><property name=\"left_attach\">1"
  "</property><property name=\"right_attach\">2</property><property name=\""
  "top_attach\">2</property><property name=\"bottom_attach\">3</property><"
  "property name=\"y_options\"/></packing></child><child><object class=\"G"
  "tkComboBox\" id=\"category-icon-size\"><property name=\"visible\">True<"
  "/property><property name=\"can_focus\">False</property><property name=\""
  "model\">icon-sizes</property><child><object class=\"GtkCellRendererText"
  "\" id=\"cellrenderertext2\"/><attributes><attribute name=\"text\">0</at"
  "tribute></attributes></child></object><packing><property name=\"left_at"
  "tach\">1</property><property name=\"right_attach\">2</property><propert"
  "y name=\"top_attach\">4</property><property name=\"bottom_attach\">5</p"
  "roperty><property name=\"y_options\"/></packing></child><child><object "
  "class=\"GtkCheckButton\" id=\"hide-window-decorations\"><property name="
  "\"label\" translatable=\"yes\">Hide window decorations</property><prope"
  "rty name=\"use_action_appearance\">False</property><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">True</property><prop"
  "erty name=\"receives_default\">False</property><property name=\"tooltip"
  "_text\" translatable=\"yes\">Hide window decorations such as title bar "
  "and borders.</property><property name=\"use_underline\">True</property>"
  "<property name=\"active\">False</property><property name=\"draw_indicat"
  "or\">True</property></object><packing><property name=\"left_attach\">0<"
  "/property><property name=\"right_attach\">2</property><property name=\""
  "top_attach\">5</property><property name=\"bottom_attach\">8</property><"
  "property name=\"y_options\"/></packing></child></object></child></objec"
  "t></child><child type=\"label\"><object class=\"GtkLabel\" id=\"label5\""
  "><property name=\"visible\">True</property><property name=\"can_focus\""
  ">False</property><property name=\"label\" translatable=\"yes\">Appearan"
  "ce</property><property name=\"use_markup\">True</property><attributes><"
  "attribute name=\"weight\" value=\"bold\"/></attributes></object></child"
  "></object><packing><property name=\"expand\">False</property><property "
  "name=\"fill\">True</property><property name=\"position\">1</property></"
  "packing></child><child><object class=\"GtkFrame\" id=\"frame2\"><proper"
  "ty name=\"visible\">True</property><property name=\"can_focus\">False</"
  "property><property name=\"label_xalign\">0</property><property name=\"s"
  "hadow_type\">none</property><child><object class=\"GtkAlignment\" id=\""
  "alignment3\"><property name=\"visible\">True</property><property name=\""
  "can_focus\">False</property><property name=\"xalign\">0</property><prop"
  "erty name=\"xscale\">0</property><property name=\"yscale\">0</property>"
  "<property name=\"top_padding\">6</property><property name=\"left_paddin"
  "g\">12</property><child><object class=\"GtkButton\" id=\"button-clear\""
  "><property name=\"label\" translatable=\"yes\">C_lear Custom Command Hi"
  "story</property><property name=\"use_action_appearance\">False</propert"
  "y><property name=\"visible\">True</property><property name=\"can_focus\""
  ">True</property><property name=\"receives_default\">True</property><pro"
  "perty name=\"image\">image3</property><property name=\"use_underline\">"
  "True</property></object></child></object></child><child type=\"label\">"
  "<object class=\"GtkLabel\" id=\"label7\"><property name=\"visible\">Tru"
  "e</property><property name=\"can_focus\">False</property><property name"
  "=\"label\" translatable=\"yes\">History</property><attributes><attribut"
  "e name=\"weight\" value=\"bold\"/></attributes></object></child></objec"
  "t><packing><property name=\"expand\">False</property><property name=\"f"
  "ill\">True</property><property name=\"position\">2</property></packing>"
  "</child></object></child><child type=\"tab\"><object class=\"GtkLabel\""
  " id=\"label1\"><property name=\"visible\">True</property><property name"
  "=\"can_focus\">False</property><property name=\"label\" translatable=\""
  "yes\">_General</property><property name=\"use_underline\">True</propert"
  "y></object><packing><property name=\"tab_fill\">False</property></packi"
  "ng></child><child><object class=\"GtkVBox\" id=\"vbox1\"><property name"
  "=\"visible\">True</property><property name=\"can_focus\">False</propert"
  "y><property name=\"border_width\">12</property><property name=\"spacing"
  "\">6</property><child><object class=\"GtkInfoBar\" id=\"infobar1\"><pro"
  "perty name=\"visible\">True</property><property name=\"can_focus\">Fals"
  "e</property><child internal-child=\"content_area\"><object class=\"GtkB"
  "ox\"><property name=\"can_focus\">False</property><child><object class="
  "\"GtkLabel\"><property name=\"visible\">True</property><property name=\""
  "can_focus\">False</property><property name=\"label\" translatable=\"yes"
  "\">Custom actions are only available in collapsed mode.</property><prop"
  "erty name=\"use_markup\">True</property><property name=\"wrap\">False</"
  "property></object><packing><property name=\"expand\">False</property><p"
  "roperty name=\"fill\">True</property><property name=\"position\">0</pro"
  "perty></packing></child></object><packing><property name=\"expand\">Fal"
  "se</property><property name=\"fill\">False</property><property name=\"p"
  "osition\">0</property></packing></child></object><packing><property nam"
  "e=\"expand\">False</property><property name=\"fill\">True</property><pr"
  "operty name=\"position\">0</property></packing></child><child><object c"
  "lass=\"GtkHBox\" id=\"hbox1\"><property name=\"visible\">True</property"
  "><property name=\"can_focus\">False</property><property name=\"spacing\""
  ">6</property><child><object class=\"GtkScrolledWindow\" id=\"scrolledwi"
  "ndow1\"><property name=\"visible\">True</property><property name=\"can_"
  "focus\">True</property><property name=\"shadow_type\">etched-in</proper"
  "ty><child><object class=\"GtkTreeView\" id=\"actions-treeview\"><proper"
  "ty name=\"visible\">True</property><property name=\"can_focus\">True</p"
  "roperty><property name=\"model\">actions-store</property><property name"
  "=\"headers_clickable\">False</property><property name=\"rules_hint\">Tr"
  "ue</property><property name=\"enable_search\">False</property><property"
  " name=\"search_column\">0</property><child internal-child=\"selection\""
  "><object class=\"GtkTreeSelection\"/></child><child><object class=\"Gtk"
  "TreeViewColumn\" id=\"treeviewcolumn1\"><property name=\"title\" transl"
  "atable=\"yes\">Pattern</property><child><object class=\"GtkCellRenderer"
  "Text\" id=\"cellrenderertext1\"/><attributes><attribute name=\"text\">0"
  "</attribute></attributes></child></object></child></object></child></ob"
  "ject><packing><property name=\"expand\">True</property><property name=\""
  "fill\">True</property><property name=\"position\">0</property></packing"
  "></child><child><object class=\"GtkAlignment\" id=\"alignment1\"><prope"
  "rty name=\"visible\">True</property><property name=\"can_focus\">False<"
  "/property><property name=\"yalign\">0</property><property name=\"xscale"
  "\">0</property><property name=\"yscale\">0</property><child><object cla"
  "ss=\"GtkVBox\" id=\"vbox2\"><property name=\"visible\">True</property><"
  "property name=\"can_focus\">False</property><property name=\"spacing\">"
  "6</property><child><object class=\"GtkButton\" id=\"button-add\"><prope"
  "rty name=\"use_action_appearance\">False</property><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">True</property><prop"
  "erty name=\"receives_default\">True</property><property name=\"tooltip_"
  "text\" translatable=\"yes\">Add a new custom action.</property><child><"
  "object class=\"GtkImage\" id=\"image1\"><property name=\"visible\">True"
  "</property><property name=\"can_focus\">False</property><property name="
  "\"icon_name\">list-add-symbolic</property></object></child></object><pa"
  "cking><property name=\"expand\">True</property><property name=\"fill\">"
  "True</property><property name=\"position\">0</property></packing></chil"
  "d><child><object class=\"GtkButton\" id=\"button-remove\"><property nam"
  "e=\"use_action_appearance\">False</property><property name=\"visible\">"
  "True</property><property name=\"can_focus\">True</property><property na"
  "me=\"receives_default\">True</property><property name=\"tooltip_text\" "
  "translatable=\"yes\">Remove the currently selected action.</property><c"
  "hild><object class=\"GtkImage\" id=\"image2\"><property name=\"visible\""
  ">True</property><property name=\"can_focus\">False</property><property "
  "name=\"icon_name\">edit-delete-symbolic</property></object></child></ob"
  "ject><packing><property name=\"expand\">True</property><property name=\""
  "fill\">True</property><property name=\"position\">1</property></packing"
  "></child></object></child></object><packing><property name=\"expand\">F"
  "alse</property><property name=\"fill\">True</property><property name=\""
  "position\">1</property></packing></child></object><packing><property na"
  "me=\"expand\">True</property><property name=\"fill\">True</property><pr"
  "operty name=\"position\">1</property></packing></child><child><object c"
  "lass=\"GtkTable\" id=\"table1\"><property name=\"visible\">True</proper"
  "ty><property name=\"can_focus\">False</property><property name=\"n_rows"
  "\">4</property><property name=\"n_columns\">2</property><property name="
  "\"column_spacing\">12</property><property name=\"row_spacing\">6</prope"
  "rty><child><object class=\"GtkEntry\" id=\"command\"><property name=\"v"
  "isible\">True</property><property name=\"can_focus\">True</property><pr"
  "operty name=\"tooltip_text\" translatable=\"yes\">If the type is set to"
  " prefix, %s will be replaced with the string after the pattern, %S with"
  " the complete entry text. For regular expressions you can use \\0 and \\"
  "&lt;num&gt;.</property><property name=\"invisible_char\">\342\200\242</"
  "property><property name=\"primary_icon_activatable\">False</property><p"
  "roperty name=\"secondary_icon_activatable\">False</property></object><p"
  "acking><property name=\"left_attach\">1</property><property name=\"righ"
  "t_attach\">2</property><property name=\"top_attach\">2</property><prope"
  "rty name=\"bottom_attach\">3</property></packing></child><child><object"
  " class=\"GtkEntry\" id=\"pattern\"><property name=\"visible\">True</pro"
  "perty><property name=\"can_focus\">True</property><property name=\"invi"
  "sible_char\">\342\200\242</property><property name=\"primary_icon_activ"
  "atable\">False</property><property name=\"secondary_icon_activatable\">"
  "False</property></object><packing><property name=\"left_attach\">1</pro"
  "perty><property name=\"right_attach\">2</property><property name=\"top_"
  "attach\">1</property><property name=\"bottom_attach\">2</property></pac"
  "king></child><child><object class=\"GtkLabel\" id=\"label4\"><property "
  "name=\"visible\">True</property><property name=\"can_focus\">False</pro"
  "perty><property name=\"label\" translatable=\"yes\">Co_mmand:</property"
  "><property name=\"use_underline\">True</property><property name=\"mnemo"
  "nic_widget\">command</property><property name=\"xalign\">0</property></"
  "object><packing><property name=\"top_attach\">2</property><property nam"
  "e=\"bottom_attach\">3</property><property name=\"x_options\">GTK_FILL</"
  "property></packing></child><child><object class=\"GtkLabel\" id=\"label"
  "3\"><property name=\"visible\">True</property><property name=\"can_focu"
  "s\">False</property><property name=\"label\" translatable=\"yes\">Patte"
  "_rn:</property><property name=\"use_underline\">True</property><propert"
  "y name=\"mnemonic_widget\">pattern</property><property name=\"xalign\">"
  "0</property></object><packing><property name=\"top_attach\">1</property"
  "><property name=\"bottom_attach\">2</property><property name=\"x_option"
  "s\">GTK_FILL</property></packing></child><child><object class=\"GtkLabe"
  "l\" id=\"label8\"><property name=\"visible\">True</property><property n"
  "ame=\"can_focus\">False</property><property name=\"label\" translatable"
  "=\"yes\">_Type:</property><property name=\"use_underline\">True</proper"
  "ty><property name=\"xalign\">0</property></object><packing><property na"
  "me=\"x_options\">GTK_FILL</property></packing></child><child><object cl"
  "ass=\"GtkCheckButton\" id=\"save\"><property name=\"label\" translatabl"
  "e=\"yes\">_Save match in command history</property><property name=\"use"
  "_action_appearance\">False</property><property name=\"visible\">True</p"
  "roperty><property name=\"can_focus\">True</property><property name=\"re"
  "ceives_default\">False</property><property name=\"use_underline\">True<"
  "/property><property name=\"draw_indicator\">True</property></object><pa"
  "cking><property name=\"left_attach\">1</property><property name=\"right"
  "_attach\">2</property><property name=\"top_attach\">3</property><proper"
  "ty name=\"bottom_attach\">4</property></packing></child><child><placeho"
  "lder/></child><child><object class=\"GtkComboBox\" id=\"type\"><propert"
  "y name=\"visible\">True</property><property name=\"can_focus\">False</p"
  "roperty><property name=\"hexpand\">True</property><property name=\"mode"
  "l\">action-types</property><child><object class=\"GtkCellRendererText\""
  " id=\"cellrenderertext3\"/><attributes><attribute name=\"text\">0</attr"
  "ibute></attributes></child></object><packing><property name=\"left_atta"
  "ch\">1</property><property name=\"right_attach\">2</property></packing>"
  "</child></object><packing><property name=\"expand\">False</property><pr"
  "operty name=\"fill\">True</property><property name=\"position\">2</prop"
  "erty></packing></child></object><packing><property name=\"position\">1<"
  "/property></packing></child><child type=\"tab\"><object class=\"GtkLabe"
  "l\" id=\"label2\"><property name=\"visible\">True</property><property n"
  "ame=\"can_focus\">False</property><property name=\"label\" translatable"
  "=\"yes\">Custom _Actions</property><property name=\"use_underline\">Tru"
  "e</property></object><packing><property name=\"position\">1</property><"
  "property name=\"tab_fill\">False</property></packing></child></object><"
  "packing><property name=\"expand\">True</property><property name=\"fill\""
  ">True</property><property name=\"position\">1</property></packing></chi"
  "ld></object></child><action-widgets><action-widget response=\"0\">butto"
  "n-close</action-widget><action-widget response=\"-11\">button-help</act"
  "ion-widget></action-widgets></object></interface>"
};

static const unsigned appfinder_preferences_ui_length = 25781u;

