/* automatically generated from appearance-dialog.glade */
#ifdef __SUNPRO_C
#pragma align 4 (appearance_dialog_ui)
#endif
#ifdef __GNUC__
static const char appearance_dialog_ui[] __attribute__ ((__aligned__ (4))) =
#else
static const char appearance_dialog_ui[] =
#endif
{
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?><interface><requires lib=\"gt"
  "k+\" version=\"3.20\"/><requires lib=\"libxfce4ui-2\" version=\"4.13\"/"
  "><object class=\"GtkFileFilter\" id=\"filefilter1\"><patterns><pattern>"
  "*.tar</pattern><pattern>*.zip</pattern></patterns></object><object clas"
  "s=\"GtkImage\" id=\"image1\"><property name=\"visible\">True</property>"
  "<property name=\"can_focus\">False</property><property name=\"icon_name"
  "\">window-close-symbolic</property></object><object class=\"GtkImage\" "
  "id=\"image2\"><property name=\"visible\">True</property><property name="
  "\"can_focus\">False</property><property name=\"icon_name\">help-browser"
  "</property></object><object class=\"GtkImage\" id=\"image3\"><property "
  "name=\"visible\">True</property><property name=\"can_focus\">False</pro"
  "perty><property name=\"icon_name\">list-add-symbolic</property></object"
  "><object class=\"GtkImage\" id=\"image4\"><property name=\"visible\">Tr"
  "ue</property><property name=\"can_focus\">False</property><property nam"
  "e=\"icon_name\">list-add-symbolic</property></object><object class=\"Gt"
  "kListStore\" id=\"liststore1\"><columns><column type=\"gchararray\"/></"
  "columns><data><row><col id=\"0\" translatable=\"yes\">None</col></row><"
  "row><col id=\"0\" translatable=\"yes\">Slight</col></row><row><col id=\""
  "0\" translatable=\"yes\">Medium</col></row><row><col id=\"0\" translata"
  "ble=\"yes\">Full</col></row></data></object><object class=\"GtkListStor"
  "e\" id=\"liststore2\"><columns><column type=\"gchararray\"/></columns><"
  "data><row><col id=\"0\" translatable=\"yes\">Icons</col></row><row><col"
  " id=\"0\" translatable=\"yes\">Text</col></row><row><col id=\"0\" trans"
  "latable=\"yes\">Text under icons</col></row><row><col id=\"0\" translat"
  "able=\"yes\">Text next to icons</col></row></data></object><object clas"
  "s=\"GtkListStore\" id=\"liststore3\"><columns><column type=\"gint\"/><c"
  "olumn type=\"gchararray\"/></columns><data><row><col id=\"0\">1</col><c"
  "ol id=\"1\" translatable=\"yes\">1x (no scaling)</col></row><row><col i"
  "d=\"0\">2</col><col id=\"1\" translatable=\"yes\">2x</col></row></data>"
  "</object><object class=\"GtkAdjustment\" id=\"xft_custom_dpi\"><propert"
  "y name=\"lower\">48</property><property name=\"upper\">1000</property><"
  "property name=\"value\">96</property><property name=\"step_increment\">"
  "1</property><property name=\"page_increment\">10</property></object><ob"
  "ject class=\"GtkListStore\" id=\"xft_rgba_store\"><columns><column type"
  "=\"GdkPixbuf\"/><column type=\"gchararray\"/></columns></object><object"
  " class=\"XfceTitledDialog\" id=\"dialog\"><property name=\"can_focus\">"
  "False</property><property name=\"title\" translatable=\"yes\">Appearanc"
  "e</property><property name=\"window_position\">center-on-parent</proper"
  "ty><property name=\"default_width\">400</property><property name=\"defa"
  "ult_height\">540</property><property name=\"icon_name\">org.xfce.settin"
  "gs.appearance</property><property name=\"type_hint\">dialog</property><"
  "child internal-child=\"vbox\"><object class=\"GtkBox\" id=\"dialog-vbox"
  "1\"><property name=\"visible\">True</property><property name=\"can_focu"
  "s\">False</property><property name=\"orientation\">vertical</property><"
  "property name=\"spacing\">2</property><child internal-child=\"action_ar"
  "ea\"><object class=\"GtkButtonBox\" id=\"dialog-action_area1\"><propert"
  "y name=\"visible\">True</property><property name=\"can_focus\">False</p"
  "roperty><property name=\"layout_style\">end</property><child><object cl"
  "ass=\"GtkButton\" id=\"button2\"><property name=\"label\" translatable="
  "\"yes\">_Help</property><property name=\"visible\">True</property><prop"
  "erty name=\"can_focus\">True</property><property name=\"receives_defaul"
  "t\">True</property><property name=\"image\">image2</property><property "
  "name=\"use_underline\">True</property></object><packing><property name="
  "\"expand\">False</property><property name=\"fill\">False</property><pro"
  "perty name=\"position\">0</property><property name=\"secondary\">True</"
  "property></packing></child><child><object class=\"GtkButton\" id=\"butt"
  "on1\"><property name=\"label\" translatable=\"yes\">_Close</property><p"
  "roperty name=\"visible\">True</property><property name=\"can_focus\">Tr"
  "ue</property><property name=\"receives_default\">True</property><proper"
  "ty name=\"image\">image1</property><property name=\"use_underline\">Tru"
  "e</property></object><packing><property name=\"expand\">False</property"
  "><property name=\"fill\">False</property><property name=\"position\">0<"
  "/property></packing></child></object><packing><property name=\"expand\""
  ">False</property><property name=\"fill\">True</property><property name="
  "\"pack_type\">end</property><property name=\"position\">0</property></p"
  "acking></child><child><object class=\"GtkNotebook\" id=\"plug-child\"><"
  "property name=\"visible\">True</property><property name=\"can_focus\">T"
  "rue</property><property name=\"border_width\">6</property><child><objec"
  "t class=\"GtkBox\"><property name=\"visible\">True</property><property "
  "name=\"can_focus\">False</property><property name=\"border_width\">12</"
  "property><property name=\"orientation\">vertical</property><child><obje"
  "ct class=\"GtkScrolledWindow\" id=\"scrolledwindow2\"><property name=\""
  "visible\">True</property><property name=\"can_focus\">True</property><p"
  "roperty name=\"vexpand\">True</property><property name=\"shadow_type\">"
  "etched-in</property><child><object class=\"GtkViewport\"><property name"
  "=\"visible\">True</property><property name=\"can_focus\">False</propert"
  "y><child><object class=\"GtkTreeView\" id=\"gtk_theme_treeview\"><prope"
  "rty name=\"visible\">True</property><property name=\"can_focus\">True</"
  "property><property name=\"headers_visible\">False</property><property n"
  "ame=\"show_expanders\">False</property><child internal-child=\"selectio"
  "n\"><object class=\"GtkTreeSelection\" id=\"treeview-selection1\"/></ch"
  "ild></object></child></object></child></object><packing><property name="
  "\"expand\">False</property><property name=\"fill\">True</property><prop"
  "erty name=\"position\">0</property></packing></child><child><object cla"
  "ss=\"GtkBox\"><property name=\"visible\">True</property><property name="
  "\"can_focus\">False</property><property name=\"orientation\">vertical</"
  "property><child><object class=\"GtkButton\" id=\"install_gtk_theme\"><p"
  "roperty name=\"label\" translatable=\"yes\">_Add</property><property na"
  "me=\"visible\">True</property><property name=\"can_focus\">True</proper"
  "ty><property name=\"receives_default\">True</property><property name=\""
  "halign\">start</property><property name=\"image\">image3</property><pro"
  "perty name=\"use_underline\">True</property></object><packing><property"
  " name=\"expand\">False</property><property name=\"fill\">True</property"
  "><property name=\"position\">0</property></packing></child><style><clas"
  "s name=\"inline-toolbar\"/></style></object><packing><property name=\"e"
  "xpand\">False</property><property name=\"fill\">True</property><propert"
  "y name=\"position\">1</property></packing></child><child><object class="
  "\"GtkBox\" id=\"xfwm4_sync\"><property name=\"visible\">True</property>"
  "<property name=\"can_focus\">False</property><property name=\"margin_to"
  "p\">12</property><child><object class=\"GtkLabel\"><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">False</property><pro"
  "perty name=\"label\" translatable=\"yes\">Set matching Xfwm4 theme if t"
  "here is one</property><property name=\"xalign\">0</property></object><p"
  "acking><property name=\"expand\">True</property><property name=\"fill\""
  ">True</property><property name=\"position\">0</property></packing></chi"
  "ld><child><object class=\"GtkSwitch\" id=\"xfwm4_sync_switch\"><propert"
  "y name=\"visible\">True</property><property name=\"can_focus\">True</pr"
  "operty></object><packing><property name=\"expand\">False</property><pro"
  "perty name=\"fill\">True</property><property name=\"position\">1</prope"
  "rty></packing></child></object><packing><property name=\"expand\">False"
  "</property><property name=\"fill\">True</property><property name=\"posi"
  "tion\">2</property></packing></child></object></child><child type=\"tab"
  "\"><object class=\"GtkLabel\" id=\"label1\"><property name=\"visible\">"
  "True</property><property name=\"can_focus\">False</property><property n"
  "ame=\"label\" translatable=\"yes\">St_yle</property><property name=\"us"
  "e_underline\">True</property></object><packing><property name=\"tab_fil"
  "l\">False</property></packing></child><child><object class=\"GtkBox\"><"
  "property name=\"visible\">True</property><property name=\"can_focus\">F"
  "alse</property><property name=\"border_width\">12</property><property n"
  "ame=\"orientation\">vertical</property><child><object class=\"GtkScroll"
  "edWindow\" id=\"scrolledwindow1\"><property name=\"visible\">True</prop"
  "erty><property name=\"can_focus\">True</property><property name=\"shado"
  "w_type\">etched-in</property><child><object class=\"GtkTreeView\" id=\""
  "icon_theme_treeview\"><property name=\"visible\">True</property><proper"
  "ty name=\"can_focus\">True</property><property name=\"headers_visible\""
  ">False</property><property name=\"show_expanders\">False</property><chi"
  "ld internal-child=\"selection\"><object class=\"GtkTreeSelection\" id=\""
  "treeview-selection2\"/></child></object></child></object><packing><prop"
  "erty name=\"expand\">True</property><property name=\"fill\">True</prope"
  "rty><property name=\"position\">0</property></packing></child><child><o"
  "bject class=\"GtkBox\"><property name=\"visible\">True</property><prope"
  "rty name=\"can_focus\">False</property><property name=\"orientation\">v"
  "ertical</property><child><object class=\"GtkButton\" id=\"install_icon_"
  "theme\"><property name=\"label\" translatable=\"yes\">_Add</property><p"
  "roperty name=\"visible\">True</property><property name=\"can_focus\">Tr"
  "ue</property><property name=\"receives_default\">True</property><proper"
  "ty name=\"halign\">start</property><property name=\"image\">image4</pro"
  "perty><property name=\"use_underline\">True</property></object><packing"
  "><property name=\"expand\">False</property><property name=\"fill\">True"
  "</property><property name=\"position\">0</property></packing></child><s"
  "tyle><class name=\"inline-toolbar\"/></style></object><packing><propert"
  "y name=\"expand\">False</property><property name=\"fill\">True</propert"
  "y><property name=\"position\">1</property></packing></child></object><p"
  "acking><property name=\"position\">1</property></packing></child><child"
  " type=\"tab\"><object class=\"GtkLabel\" id=\"label2\"><property name=\""
  "visible\">True</property><property name=\"can_focus\">False</property><"
  "property name=\"label\" translatable=\"yes\">_Icons</property><property"
  " name=\"use_underline\">True</property></object><packing><property name"
  "=\"position\">1</property><property name=\"tab_fill\">False</property><"
  "/packing></child><child><object class=\"GtkScrolledWindow\"><property n"
  "ame=\"visible\">True</property><property name=\"can_focus\">True</prope"
  "rty><child><object class=\"GtkViewport\"><property name=\"visible\">Tru"
  "e</property><property name=\"can_focus\">False</property><child><object"
  " class=\"GtkBox\" id=\"vbox3\"><property name=\"visible\">True</propert"
  "y><property name=\"can_focus\">False</property><property name=\"border_"
  "width\">12</property><property name=\"orientation\">vertical</property>"
  "<property name=\"spacing\">18</property><child><object class=\"GtkFrame"
  "\" id=\"frame3\"><property name=\"visible\">True</property><property na"
  "me=\"can_focus\">False</property><property name=\"label_xalign\">0</pro"
  "perty><property name=\"shadow_type\">none</property><child><object clas"
  "s=\"GtkAlignment\" id=\"alignment3\"><property name=\"visible\">True</p"
  "roperty><property name=\"can_focus\">False</property><property name=\"t"
  "op_padding\">6</property><property name=\"left_padding\">12</property><"
  "child><object class=\"GtkFontButton\" id=\"gtk_fontname_button\"><prope"
  "rty name=\"visible\">True</property><property name=\"can_focus\">True</"
  "property><property name=\"receives_default\">True</property><property n"
  "ame=\"tooltip_text\" translatable=\"yes\">This font will be used as the"
  " default font used when drawing user interface text</property><property"
  " name=\"font\">Sans 12</property><property name=\"title\" translatable="
  "\"yes\">Select a default font</property></object></child></object></chi"
  "ld><child type=\"label\"><object class=\"GtkLabel\" id=\"label7\"><prop"
  "erty name=\"visible\">True</property><property name=\"can_focus\">False"
  "</property><property name=\"label\" translatable=\"yes\">Default Fon_t<"
  "/property><property name=\"use_underline\">True</property><property nam"
  "e=\"mnemonic_widget\">gtk_fontname_button</property><attributes><attrib"
  "ute name=\"weight\" value=\"bold\"/></attributes></object></child></obj"
  "ect><packing><property name=\"expand\">False</property><property name=\""
  "fill\">True</property><property name=\"position\">0</property></packing"
  "></child><child><object class=\"GtkFrame\" id=\"frame7\"><property name"
  "=\"visible\">True</property><property name=\"can_focus\">False</propert"
  "y><property name=\"label_xalign\">0</property><property name=\"shadow_t"
  "ype\">none</property><child><object class=\"GtkAlignment\" id=\"alignme"
  "nt7\"><property name=\"visible\">True</property><property name=\"can_fo"
  "cus\">False</property><property name=\"top_padding\">6</property><prope"
  "rty name=\"left_padding\">12</property><child><object class=\"GtkFontBu"
  "tton\" id=\"gtk_monospace_fontname_button\"><property name=\"visible\">"
  "True</property><property name=\"can_focus\">True</property><property na"
  "me=\"receives_default\">True</property><property name=\"tooltip_text\" "
  "translatable=\"yes\">This font will be used as the default monospace fo"
  "nt, for example by terminal emulators.</property><property name=\"font\""
  ">Sans 12</property><property name=\"title\" translatable=\"yes\">Select"
  " a default monospace font</property></object></child></object></child><"
  "child type=\"label\"><object class=\"GtkLabel\" id=\"label14\"><propert"
  "y name=\"visible\">True</property><property name=\"can_focus\">False</p"
  "roperty><property name=\"label\" translatable=\"yes\">Default _Monospac"
  "e Font</property><property name=\"use_underline\">True</property><prope"
  "rty name=\"mnemonic_widget\">gtk_monospace_fontname_button</property><a"
  "ttributes><attribute name=\"weight\" value=\"bold\"/></attributes></obj"
  "ect></child></object><packing><property name=\"expand\">False</property"
  "><property name=\"fill\">True</property><property name=\"position\">1</"
  "property></packing></child><child><object class=\"GtkFrame\" id=\"frame"
  "4\"><property name=\"visible\">True</property><property name=\"can_focu"
  "s\">False</property><property name=\"label_xalign\">0</property><proper"
  "ty name=\"shadow_type\">none</property><child><object class=\"GtkAlignm"
  "ent\"><property name=\"visible\">True</property><property name=\"can_fo"
  "cus\">False</property><property name=\"top_padding\">6</property><prope"
  "rty name=\"left_padding\">12</property><child><object class=\"GtkBox\">"
  "<property name=\"visible\">True</property><property name=\"can_focus\">"
  "False</property><property name=\"orientation\">vertical</property><prop"
  "erty name=\"spacing\">6</property><child><object class=\"GtkCheckButton"
  "\" id=\"xft_antialias_check_button\"><property name=\"label\" translata"
  "ble=\"yes\">_Enable anti-aliasing</property><property name=\"visible\">"
  "True</property><property name=\"can_focus\">True</property><property na"
  "me=\"receives_default\">False</property><property name=\"tooltip_text\""
  " translatable=\"yes\">Anti-aliasing, or font smoothing, can improve the"
  " look of text on the screen</property><property name=\"halign\">start</"
  "property><property name=\"use_underline\">True</property><property name"
  "=\"active\">True</property><property name=\"draw_indicator\">True</prop"
  "erty></object><packing><property name=\"expand\">False</property><prope"
  "rty name=\"fill\">True</property><property name=\"position\">0</propert"
  "y></packing></child><child><object class=\"GtkAlignment\"><property nam"
  "e=\"visible\">True</property><property name=\"can_focus\">False</proper"
  "ty><property name=\"left_padding\">12</property><child><object class=\""
  "GtkGrid\" id=\"grid1\"><property name=\"visible\">True</property><prope"
  "rty name=\"can_focus\">False</property><property name=\"row_spacing\">6"
  "</property><property name=\"column_spacing\">12</property><child><objec"
  "t class=\"GtkLabel\" id=\"label10\"><property name=\"visible\">True</pr"
  "operty><property name=\"can_focus\">False</property><property name=\"ha"
  "lign\">start</property><property name=\"label\" translatable=\"yes\">Hi"
  "ntin_g:</property><property name=\"use_underline\">True</property></obj"
  "ect><packing><property name=\"left_attach\">0</property><property name="
  "\"top_attach\">0</property></packing></child><child><object class=\"Gtk"
  "ComboBox\" id=\"xft_hinting_style_combo_box\"><property name=\"visible\""
  ">True</property><property name=\"can_focus\">False</property><property "
  "name=\"tooltip_text\" translatable=\"yes\">Many fonts contain informati"
  "on that provides extra hints as to how best draw the font; pick whichev"
  "er looks best according to personal preference</property><property name"
  "=\"hexpand\">True</property><property name=\"model\">liststore1</proper"
  "ty><child><object class=\"GtkCellRendererText\" id=\"cellrenderertext1\""
  "/><attributes><attribute name=\"text\">0</attribute></attributes></chil"
  "d></object><packing><property name=\"left_attach\">1</property><propert"
  "y name=\"top_attach\">0</property></packing></child><child><object clas"
  "s=\"GtkLabel\" id=\"label11\"><property name=\"visible\">True</property"
  "><property name=\"can_focus\">False</property><property name=\"halign\""
  ">start</property><property name=\"label\" translatable=\"yes\">Sub-_pix"
  "el order:</property><property name=\"use_underline\">True</property></o"
  "bject><packing><property name=\"left_attach\">0</property><property nam"
  "e=\"top_attach\">1</property></packing></child><child><object class=\"G"
  "tkComboBox\" id=\"xft_rgba_combo_box\"><property name=\"visible\">True<"
  "/property><property name=\"can_focus\">False</property><property name=\""
  "tooltip_text\" translatable=\"yes\">Font quality on a TFT or LCD screen"
  " can be greatly improved by choosing the correct sub-pixel order of the"
  " screen</property><property name=\"hexpand\">True</property><property n"
  "ame=\"model\">xft_rgba_store</property><child><object class=\"GtkCellRe"
  "ndererText\" id=\"cellrenderertext3\"/><attributes><attribute name=\"te"
  "xt\">1</attribute></attributes></child><child><object class=\"GtkCellRe"
  "ndererPixbuf\" id=\"cellrendererpixbuf1\"/><attributes><attribute name="
  "\"pixbuf\">0</attribute></attributes></child></object><packing><propert"
  "y name=\"left_attach\">1</property><property name=\"top_attach\">1</pro"
  "perty></packing></child></object></child></object><packing><property na"
  "me=\"expand\">False</property><property name=\"fill\">True</property><p"
  "roperty name=\"position\">1</property></packing></child></object></chil"
  "d></object></child><child type=\"label\"><object class=\"GtkLabel\" id="
  "\"label8\"><property name=\"visible\">True</property><property name=\"c"
  "an_focus\">False</property><property name=\"label\" translatable=\"yes\""
  ">Rendering</property><attributes><attribute name=\"weight\" value=\"bol"
  "d\"/></attributes></object></child></object><packing><property name=\"e"
  "xpand\">False</property><property name=\"fill\">True</property><propert"
  "y name=\"position\">2</property></packing></child><child><object class="
  "\"GtkFrame\" id=\"frame5\"><property name=\"visible\">True</property><p"
  "roperty name=\"can_focus\">False</property><property name=\"label_xalig"
  "n\">0</property><property name=\"shadow_type\">none</property><child><o"
  "bject class=\"GtkAlignment\" id=\"alignment5\"><property name=\"visible"
  "\">True</property><property name=\"can_focus\">False</property><propert"
  "y name=\"top_padding\">6</property><property name=\"left_padding\">12</"
  "property><child><object class=\"GtkBox\" id=\"hbox1\"><property name=\""
  "visible\">True</property><property name=\"can_focus\">False</property><"
  "property name=\"spacing\">12</property><child><object class=\"GtkCheckB"
  "utton\" id=\"xft_custom_dpi_check_button\"><property name=\"label\" tra"
  "nslatable=\"yes\">Custom _DPI setting:</property><property name=\"visib"
  "le\">True</property><property name=\"can_focus\">True</property><proper"
  "ty name=\"receives_default\">False</property><property name=\"tooltip_t"
  "ext\" translatable=\"yes\">Override the detected monitor resolution if "
  "fonts look too large or too small</property><property name=\"use_underl"
  "ine\">True</property><property name=\"draw_indicator\">True</property><"
  "/object><packing><property name=\"expand\">False</property><property na"
  "me=\"fill\">True</property><property name=\"position\">0</property></pa"
  "cking></child><child><object class=\"GtkSpinButton\" id=\"xft_custom_dp"
  "i_spin_button\"><property name=\"visible\">True</property><property nam"
  "e=\"sensitive\">False</property><property name=\"can_focus\">True</prop"
  "erty><property name=\"width_chars\">6</property><property name=\"primar"
  "y_icon_activatable\">False</property><property name=\"secondary_icon_ac"
  "tivatable\">False</property><property name=\"adjustment\">xft_custom_dp"
  "i</property><property name=\"snap_to_ticks\">True</property><property n"
  "ame=\"numeric\">True</property><property name=\"update_policy\">if-vali"
  "d</property></object><packing><property name=\"expand\">False</property"
  "><property name=\"fill\">True</property><property name=\"position\">1</"
  "property></packing></child></object></child></object></child><child typ"
  "e=\"label\"><object class=\"GtkLabel\" id=\"label9\"><property name=\"v"
  "isible\">True</property><property name=\"can_focus\">False</property><p"
  "roperty name=\"label\" translatable=\"yes\">DPI</property><attributes><"
  "attribute name=\"weight\" value=\"bold\"/></attributes></object></child"
  "></object><packing><property name=\"expand\">False</property><property "
  "name=\"fill\">True</property><property name=\"position\">3</property></"
  "packing></child></object></child></object></child></object><packing><pr"
  "operty name=\"position\">2</property></packing></child><child type=\"ta"
  "b\"><object class=\"GtkLabel\" id=\"label3\"><property name=\"visible\""
  ">True</property><property name=\"can_focus\">False</property><property "
  "name=\"label\" translatable=\"yes\">_Fonts</property><property name=\"u"
  "se_underline\">True</property></object><packing><property name=\"positi"
  "on\">2</property><property name=\"tab_fill\">False</property></packing>"
  "</child><child><object class=\"GtkScrolledWindow\"><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">True</property><chil"
  "d><object class=\"GtkViewport\"><property name=\"visible\">True</proper"
  "ty><property name=\"can_focus\">False</property><child><object class=\""
  "GtkBox\" id=\"vbox1\"><property name=\"visible\">True</property><proper"
  "ty name=\"can_focus\">False</property><property name=\"border_width\">1"
  "2</property><property name=\"orientation\">vertical</property><property"
  " name=\"spacing\">18</property><child><object class=\"GtkFrame\" id=\"f"
  "rame1\"><property name=\"visible\">True</property><property name=\"can_"
  "focus\">False</property><property name=\"label_xalign\">0</property><pr"
  "operty name=\"shadow_type\">none</property><child><object class=\"GtkAl"
  "ignment\" id=\"alignment1\"><property name=\"visible\">True</property><"
  "property name=\"can_focus\">False</property><property name=\"top_paddin"
  "g\">6</property><property name=\"left_padding\">12</property><child><ob"
  "ject class=\"GtkBox\" id=\"vbox2\"><property name=\"visible\">True</pro"
  "perty><property name=\"can_focus\">False</property><property name=\"ori"
  "entation\">vertical</property><property name=\"spacing\">6</property><c"
  "hild><object class=\"GtkCheckButton\" id=\"gtk_button_images_check_butt"
  "on\"><property name=\"label\" translatable=\"yes\">Show images on _butt"
  "ons</property><property name=\"visible\">True</property><property name="
  "\"can_focus\">True</property><property name=\"receives_default\">False<"
  "/property><property name=\"tooltip_text\" translatable=\"yes\">Specify "
  "whether icons should be displayed next to text in buttons</property><pr"
  "operty name=\"halign\">start</property><property name=\"use_underline\""
  ">True</property><property name=\"draw_indicator\">True</property></obje"
  "ct><packing><property name=\"expand\">True</property><property name=\"f"
  "ill\">True</property><property name=\"position\">0</property></packing>"
  "</child><child><object class=\"GtkCheckButton\" id=\"gtk_menu_images_ch"
  "eck_button\"><property name=\"label\" translatable=\"yes\">Show images "
  "in _menus</property><property name=\"visible\">True</property><property"
  " name=\"can_focus\">True</property><property name=\"receives_default\">"
  "False</property><property name=\"tooltip_text\" translatable=\"yes\">Sp"
  "ecify whether icons should be displayed next to items in menus</propert"
  "y><property name=\"halign\">start</property><property name=\"use_underl"
  "ine\">True</property><property name=\"draw_indicator\">True</property><"
  "/object><packing><property name=\"expand\">True</property><property nam"
  "e=\"fill\">True</property><property name=\"position\">1</property></pac"
  "king></child><child><object class=\"GtkCheckButton\" id=\"gtk_caneditac"
  "cels_check_button\"><property name=\"label\" translatable=\"yes\">Enabl"
  "e e_ditable accelerators</property><property name=\"visible\">True</pro"
  "perty><property name=\"can_focus\">True</property><property name=\"rece"
  "ives_default\">False</property><property name=\"tooltip_text\" translat"
  "able=\"yes\">If selected, keyboard shortcuts for menu items can be chan"
  "ged by hovering the mouse over the menu item and pressing the new key c"
  "ombination for the shortcut</property><property name=\"halign\">start</"
  "property><property name=\"use_underline\">True</property><property name"
  "=\"draw_indicator\">True</property></object><packing><property name=\"e"
  "xpand\">True</property><property name=\"fill\">True</property><property"
  " name=\"position\">2</property></packing></child><child><object class=\""
  "GtkCheckButton\" id=\"gtk_dialog_button_header_check_button\"><property"
  " name=\"label\" translatable=\"yes\">Enable header bars in dialo_gs</pr"
  "operty><property name=\"visible\">True</property><property name=\"can_f"
  "ocus\">True</property><property name=\"receives_default\">False</proper"
  "ty><property name=\"tooltip_text\" translatable=\"yes\">Defines whether"
  " GTK dialogs may place widgets such as buttons in the dialog\'s header "
  "bar. Implies in Client Side Decoration. Only affects newly opened dialo"
  "gs.</property><property name=\"halign\">start</property><property name="
  "\"use_underline\">True</property><property name=\"draw_indicator\">True"
  "</property></object><packing><property name=\"expand\">True</property><"
  "property name=\"fill\">True</property><property name=\"position\">3</pr"
  "operty></packing></child></object></child></object></child><child type="
  "\"label\"><object class=\"GtkLabel\" id=\"label5\"><property name=\"vis"
  "ible\">True</property><property name=\"can_focus\">False</property><pro"
  "perty name=\"label\" translatable=\"yes\">Menus and Buttons</property><"
  "attributes><attribute name=\"weight\" value=\"bold\"/></attributes></ob"
  "ject></child></object><packing><property name=\"expand\">False</propert"
  "y><property name=\"fill\">True</property><property name=\"position\">1<"
  "/property></packing></child><child><object class=\"GtkFrame\" id=\"even"
  "t_sounds_frame\"><property name=\"can_focus\">False</property><property"
  " name=\"label_xalign\">0</property><property name=\"shadow_type\">none<"
  "/property><child><object class=\"GtkAlignment\" id=\"alignment6\"><prop"
  "erty name=\"visible\">True</property><property name=\"can_focus\">False"
  "</property><property name=\"top_padding\">6</property><property name=\""
  "left_padding\">12</property><child><object class=\"GtkBox\" id=\"vbox4\""
  "><property name=\"visible\">True</property><property name=\"can_focus\""
  ">False</property><property name=\"orientation\">vertical</property><pro"
  "perty name=\"spacing\">6</property><child><object class=\"GtkCheckButto"
  "n\" id=\"enable_event_sounds_check_button\"><property name=\"label\" tr"
  "anslatable=\"yes\">Enable _event sounds</property><property name=\"visi"
  "ble\">True</property><property name=\"can_focus\">True</property><prope"
  "rty name=\"receives_default\">False</property><property name=\"tooltip_"
  "text\" translatable=\"yes\">Enable or disable event sounds globally (re"
  "quires \"Canberra\" support)</property><property name=\"halign\">start<"
  "/property><property name=\"use_underline\">True</property><property nam"
  "e=\"draw_indicator\">True</property></object><packing><property name=\""
  "expand\">False</property><property name=\"fill\">True</property><proper"
  "ty name=\"position\">0</property></packing></child><child><object class"
  "=\"GtkCheckButton\" id=\"enable_input_feedback_sounds_button\"><propert"
  "y name=\"label\" translatable=\"yes\">Enable input feedbac_k sounds</pr"
  "operty><property name=\"visible\">True</property><property name=\"can_f"
  "ocus\">True</property><property name=\"receives_default\">False</proper"
  "ty><property name=\"tooltip_text\" translatable=\"yes\">Specify whether"
  " mouse clicks and other user input will cause event sounds to play</pro"
  "perty><property name=\"halign\">start</property><property name=\"use_un"
  "derline\">True</property><property name=\"draw_indicator\">True</proper"
  "ty></object><packing><property name=\"expand\">False</property><propert"
  "y name=\"fill\">True</property><property name=\"position\">3</property>"
  "</packing></child></object></child></object></child><child type=\"label"
  "\"><object class=\"GtkLabel\" id=\"label4\"><property name=\"visible\">"
  "True</property><property name=\"can_focus\">False</property><property n"
  "ame=\"label\" translatable=\"yes\">Event sounds</property><attributes><"
  "attribute name=\"weight\" value=\"bold\"/></attributes></object></child"
  "></object><packing><property name=\"expand\">False</property><property "
  "name=\"fill\">True</property><property name=\"position\">2</property></"
  "packing></child><child><object class=\"GtkFrame\" id=\"frame6\"><proper"
  "ty name=\"visible\">True</property><property name=\"can_focus\">False</"
  "property><property name=\"label_xalign\">0</property><property name=\"s"
  "hadow_type\">none</property><child><object class=\"GtkAlignment\" id=\""
  "alignment4\"><property name=\"visible\">True</property><property name=\""
  "can_focus\">False</property><property name=\"top_padding\">6</property>"
  "<property name=\"left_padding\">12</property><child><object class=\"Gtk"
  "ComboBox\" id=\"gdk_window_scaling_factor_combo_box\"><property name=\""
  "visible\">True</property><property name=\"can_focus\">False</property><"
  "property name=\"tooltip_text\" translatable=\"yes\">Adjust the system-w"
  "ide display scaling</property><property name=\"model\">liststore3</prop"
  "erty><property name=\"active\">0</property><property name=\"id_column\""
  ">0</property><child><object class=\"GtkCellRendererText\" id=\"cellrend"
  "erertext4\"/><attributes><attribute name=\"text\">1</attribute></attrib"
  "utes></child></object></child></object></child><child type=\"label\"><o"
  "bject class=\"GtkLabel\" id=\"label13\"><property name=\"visible\">True"
  "</property><property name=\"can_focus\">False</property><property name="
  "\"label\" translatable=\"yes\">_Window Scaling</property><property name"
  "=\"use_underline\">True</property><property name=\"mnemonic_widget\">gd"
  "k_window_scaling_factor_combo_box</property><attributes><attribute name"
  "=\"weight\" value=\"bold\"/></attributes></object></child></object><pac"
  "king><property name=\"expand\">False</property><property name=\"fill\">"
  "True</property><property name=\"position\">3</property></packing></chil"
  "d></object></child></object></child></object><packing><property name=\""
  "position\">3</property></packing></child><child type=\"tab\"><object cl"
  "ass=\"GtkLabel\" id=\"label12\"><property name=\"visible\">True</proper"
  "ty><property name=\"can_focus\">False</property><property name=\"label\""
  " translatable=\"yes\">Setti_ngs</property><property name=\"use_underlin"
  "e\">True</property></object><packing><property name=\"position\">3</pro"
  "perty><property name=\"tab_fill\">False</property></packing></child></o"
  "bject><packing><property name=\"expand\">True</property><property name="
  "\"fill\">True</property><property name=\"position\">1</property></packi"
  "ng></child></object></child><action-widgets><action-widget response=\"-"
  "11\">button2</action-widget><action-widget response=\"0\">button1</acti"
  "on-widget></action-widgets></object></interface>"
};

static const unsigned appearance_dialog_ui_length = 30461u;
