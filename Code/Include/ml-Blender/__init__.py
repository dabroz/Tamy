bl_info = {
	"name": "Tamy exporter",
	"author": "Piotr Trochim",
	"blender": (2, 5, 7),
	"location": "File > Import-Export",
	"description": "Export Tamy models and animations",
	"warning": "",
	"wiki_url": "http://code.google.com/p/tamy/wiki/TableOfContents?tm=6",
	"tracker_url": "",
	"support": 'OFFICIAL',
	"category": "Import-Export"}

if "bpy" in locals():
	import imp
	if "export_tamy" in locals():
		imp.reload(export_tamy)


import bpy
from bpy.props import StringProperty, FloatProperty, BoolProperty, EnumProperty

import os
from bpy_extras.io_utils import ( ImportHelper, ExportHelper, axis_conversion, )
	
### ===========================================================================

class ExportTamy( bpy.types.Operator, ExportHelper ):
	'''Export to Tamy file format'''

	def getArmaturesList(self, context):

		armaturesList = []
		armaturesList.append( ("None", "None", "", "", 0 ) ) # append an empty option

		idx = 1
		for object in context.scene.objects:
			if object.type == 'ARMATURE':
				armaturesList.append( ( object.name, object.name, object.name, "", idx ) )
				idx = idx + 1

		return armaturesList


	bl_idname = "export_scene.tamy"
	bl_label = 'Export Tamy'

	filename_ext = ".tsc"
	filter_glob = StringProperty(
			default="*.tsc",
			options={'HIDDEN'},
			)
			
	filesystemRoot = bpy.props.StringProperty(
			name="Filesystem root",
			description="Filesystem root directory",
			default=os.environ['ASSETS_ROOT'] )
			
	bUseSelection = BoolProperty(
			name="Selection Only",
			description="Export selected objects only",
			default=False,
			)

	selectedArmature = EnumProperty(
			name="Animated armature",
			description="If specified, export all actions as animations library of this armature",
			items=getArmaturesList
			)
		
	saveAnimations = BoolProperty(
			name="Save animations",
			description="Saves animations",
			default=True
			)

	saveMaterials = BoolProperty(
			name="Save materials",
			description="Saves materials",
			default=True
			)

	saveMeshes = BoolProperty(
			name="Save meshes",
			description="Saves meshes",
			default=True
			)

	savePrefabs = BoolProperty(
			name="Save prefabs",
			description="Saves prefabs",
			default=True
			)

	saveObjects = BoolProperty(
			name="Save objects",
			description="Saves objects",
			default=True
			)

	def execute(self, context):
		from . import export_tamy

		keywords = self.as_keywords( ignore=( "filter_glob", "check_existing", ) )
		
		keywords["filesystemRoot"] = self.filesystemRoot

		return export_tamy.export_scene( self, context, **keywords )

### ===========================================================================
	
# Add to a menu
def menu_func_export(self, context):
	self.layout.operator(ExportTamy.bl_idname, text="Tamy scene (.tsc)")

def register():
	bpy.utils.register_module(__name__)

	bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
	bpy.utils.unregister_module(__name__)

	bpy.types.INFO_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
	register()
