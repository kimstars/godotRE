/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/

#include "register_types.h"
#include "core/object/class_db.h"

#include "bytecode/bytecode_versions.h"
#include "editor/gdre_editor.h"
#include "utility/gdre_cli_main.h"
#include "utility/gdre_settings.h"
#include "utility/import_exporter.h"
#include "utility/oggstr_loader_compat.h"
#include "utility/pck_dumper.h"
#include "utility/texture_loader_compat.h"

#ifdef TOOLS_ENABLED
void gdsdecomp_init_callback() {
	EditorNode *editor = EditorNode::get_singleton();
	editor->add_child(memnew(GodotREEditor(editor)));
};
#endif

void initialize_gdsdecomp_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	ClassDB::register_abstract_class<GDScriptDecomp>();
	register_decomp_versions();

	ClassDB::register_class<GodotREEditorStandalone>();
	ClassDB::register_class<PckDumper>();
	ClassDB::register_class<ImportInfo>();
	ClassDB::register_class<ImportExporter>();
	ClassDB::register_class<OggStreamLoaderCompat>();
	ClassDB::register_class<TextureLoaderCompat>();
	ClassDB::register_class<GDRECLIMain>();

	ClassDB::register_class<PackDialog>();
	ClassDB::register_class<NewPackDialog>();
	ClassDB::register_class<ScriptCompDialog>();
	ClassDB::register_class<ScriptDecompDialog>();

#ifdef TOOLS_ENABLED
	EditorNode::add_init_callback(&gdsdecomp_init_callback);
#endif
}

void uninitialize_gdsdecomp_module(ModuleInitializationLevel p_level) {
	//NOP
}
