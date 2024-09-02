#!/usr/bin/python3
#
# Legal Stuff:
#
# This file is part of the Pop Icon Theme and is free software; you can 
# redistribute it and/or modify it under  the terms of the GNU Lesser General
# Public License as published by the Free Software Foundation; version 3.
#
# This file is part of the Pop Icon Theme and is distributed in the hope that 
# it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty 
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser 
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, see <https://www.gnu.org/licenses/lgpl-3.0.txt>
#
#
# Thanks to the GNOME icon developers for the original version of this script

import glob
import os
import shutil
import sys
import tempfile
import xml.sax
import subprocess
import argparse

from pathlib import Path

INKSCAPE = Path('/usr/bin/inkscape')
SCOUR = Path('/usr/bin/scour')
HAS_SCOUR = os.path.exists(SCOUR)
SVGO = Path('/usr/local/bin/svgo')
HAS_SVGO = os.path.exists(SVGO)
MAINDIR = Path('../../Pop')
SVGO_CONFIG = MAINDIR / '..' / 'svgo.config.js'
CLI_OUTPUT=subprocess.DEVNULL
# CLI_OUTPUT=None # Uncomment for verbose mode

SOURCES = ('actions', 'apps', 'categories', 'devices', 'emblems', 'logos', 'mimetypes', 'places', 'preferences', 'status')
INKSCAPE_ACTIONS = [
	'select-all:all',
	'unselect-by-id:crop',
	'unselect-by-id:rect',
	'EditDelete',
	'vacuum-defs',
	'FileSave',
	'FileQuit'
]
# the resolution that non-hi-dpi icons are rendered at
DPI_1_TO_1 = 96
# DPI multipliers to render at
DPIS = [1]
TEMP_DIR = tempfile.gettempdir()

inkscape_process = None

def main(args, SRC):

	def inkscape_render_rect(icon_file, rect, dpi, icon_name, size, output_file):

		crop_id = f'{icon_name}-{size}'

		temp_file = f'{TEMP_DIR}/{icon_name}.svg'
		shutil.copyfile(icon_file, temp_file)

		actions = '--actions='
		for action in INKSCAPE_ACTIONS:
			if action == 'unselect-by-id:crop':
				action = f'unselect-by-id:{crop_id}'
			if action == 'unselect-by-id:rect':
				action = f'unselect-by-id:{rect}'
			actions += f'{action};'

		cmd1 = [
			INKSCAPE,
			'-g',
			actions,
			temp_file # input file
		]
		cmd2 = [
			INKSCAPE,
			temp_file,
			'-d', str(dpi), # export-dpi
			'-i', rect, # export-id
			'-o', f'{output_file}' # export-filename
		]

		if CLI_OUTPUT == None:
			print(f'Rendering {output_file} from {icon_file}')
			print(f'Running {cmd1}')
			print(f'Running {cmd2}')
		
		try:
			subprocess.run(cmd1, check=True, stderr=CLI_OUTPUT, stdout=CLI_OUTPUT)
			subprocess.run(cmd2, check=True, stderr=CLI_OUTPUT, stdout=CLI_OUTPUT)
		except subprocess.CalledProcessError:
			print(f'Could not render {output_file}: see output')
			sys.exit(1)
	
	def scour_clean_svg(icon_file):
		out_file = Path(icon_file)
		in_file = Path(f'{icon_file}-unop')
		shutil.copy(out_file, in_file)
		cmd = [
			SCOUR,
			f'-i', in_file,
			f'-o', out_file,
			'--enable-viewboxing',
			'--enable-id-stripping',
			'--enable-comment-stripping',
			'--shorten-ids',
			'--indent=none'
		]
		if CLI_OUTPUT == None:
			print(f'Cleaning up {out_file}')
			print(f'Running {cmd}')
		try:
			if in_file.exists():
				subprocess.run(cmd, check=True, stderr=CLI_OUTPUT, stdout=CLI_OUTPUT)
		except subprocess.CalledProcessError:
			print(f'Could not clean up {icon_file}: see output')
			sys.exit(1)
		os.remove(in_file)
	
	def svgo_optimize_svgs(icon_file):
		cmd = [
			SVGO,
			f'--config={SVGO_CONFIG}',
			f'--input={icon_file}',
			f'--output={icon_file}',
		]

		if CLI_OUTPUT == None:
			print(f'Optimizing {icon_file}')
			print(f'Running {cmd}')
		try:
			subprocess.run(cmd, check=True, stderr=CLI_OUTPUT, stdout=CLI_OUTPUT)
		except subprocess.CalledProcessError:
			print(f'Could not optimize {icon_file}: see output')
			sys.exit(1)


	class ContentHandler(xml.sax.ContentHandler):
		ROOT = 0
		SVG = 1
		LAYER = 2
		OTHER = 3
		TEXT = 4
		def __init__(self, path, force=False, filter=None):
			self.stack = [self.ROOT]
			self.inside = [self.ROOT]
			self.path = path
			self.rects = []
			self.state = self.ROOT
			self.chars = ""
			self.force = force
			self.filter = filter

		def endDocument(self):
			pass

		def startElement(self, name, attrs):
			if self.inside[-1] == self.ROOT:
				if name == "svg":
					self.stack.append(self.SVG)
					self.inside.append(self.SVG)
					return
			elif self.inside[-1] == self.SVG:
				if (name == "g" and ('inkscape:groupmode' in attrs) and ('inkscape:label' in attrs)
				   and attrs['inkscape:groupmode'] == 'layer' and attrs['inkscape:label'].startswith('Baseplate')):
					self.stack.append(self.LAYER)
					self.inside.append(self.LAYER)
					self.context = None
					self.icon_name = None
					self.rects = []
					return
			elif self.inside[-1] == self.LAYER:
				if name == "text" and ('inkscape:label' in attrs) and attrs['inkscape:label'] == 'context':
					self.stack.append(self.TEXT)
					self.inside.append(self.TEXT)
					self.text='context'
					self.chars = ""
					return
				elif name == "text" and ('inkscape:label' in attrs) and attrs['inkscape:label'] == 'icon-name':
					self.stack.append(self.TEXT)
					self.inside.append(self.TEXT)
					self.text='icon-name'
					self.chars = ""
					return
				elif name == "rect":
					self.rects.append(attrs)

			self.stack.append(self.OTHER)


		def endElement(self, name):
			stacked = self.stack.pop()
			if self.inside[-1] == stacked:
				self.inside.pop()

			if stacked == self.TEXT and self.text is not None:
				assert self.text in ['context', 'icon-name']
				if self.text == 'context':
					self.context = self.chars
				elif self.text == 'icon-name':
					self.icon_name = self.chars
				self.text = None
			elif stacked == self.LAYER:
				assert self.icon_name
				assert self.context

				if self.filter is not None and not self.icon_name in self.filter:
					return

				print('   **',self.context, self.icon_name)
				sys.stdout.write('      ')
				for rect in self.rects:
					for dpi_factor in DPIS:
						width = rect['width']
						height = rect['height']
						id = rect['id']
						dpi = DPI_1_TO_1 * dpi_factor

						size_str = "%sx%s" % (width, height)
						if dpi_factor != 1:
							size_str += "@%sx" % dpi_factor

						dir = os.path.join(MAINDIR, size_str, self.context)
						outfile = os.path.join(dir, self.icon_name+'.svg')
						if not os.path.exists(dir):
							os.makedirs(dir)
						# Do a time based check!
						if self.force or not os.path.exists(outfile):
							inkscape_render_rect(self.path, id, dpi, self.icon_name, width, outfile)
							sys.stdout.write('r')
							if HAS_SCOUR:
								scour_clean_svg(outfile)
								sys.stdout.write('s')
							if HAS_SVGO:
								svgo_optimize_svgs(outfile)
								sys.stdout.write('o')
							sys.stdout.write('.')
						else:
							stat_in = os.stat(self.path)
							stat_out = os.stat(outfile)
							if stat_in.st_mtime > stat_out.st_mtime:
								inkscape_render_rect(self.path, id, dpi, self.icon_name, width, outfile)
								sys.stdout.write('r')
								if HAS_SCOUR:
									scour_clean_svg(outfile)
									sys.stdout.write('s')
								if HAS_SVGO:
									svgo_optimize_svgs(outfile)
									sys.stdout.write('o')
								sys.stdout.write('.')
							else:
								sys.stdout.write('-')
						sys.stdout.flush()
				sys.stdout.write('\n')
				sys.stdout.flush()

		def characters(self, chars):
			self.chars += chars.strip()


	if not args.svg:
		if not os.path.exists(MAINDIR):
			os.mkdir(MAINDIR)
		print ('')
		print ('  ** Rendering from SVGs in', SRC)
		print ('')
		for file in os.listdir(SRC):
			if file[-4:] == '.svg':
				file = os.path.join(SRC, file)
				handler = ContentHandler(file)
				xml.sax.parse(open(file), handler)
		print ('')
	else:
		file = os.path.join(SRC, args.svg + '.svg')

		if os.path.exists(os.path.join(file)):
			handler = ContentHandler(file, True, filter=args.filter)
			xml.sax.parse(open(file), handler)
		else:
			# icon not in this directory, try the next one
			pass

parser = argparse.ArgumentParser(description='Render icons from SVG to PNG')

parser.add_argument('svg', type=str, nargs='?', metavar='SVG',
					help="Optional SVG names (without extensions) to render. If not given, render all icons")
parser.add_argument('filter', type=str, nargs='?', metavar='FILTER',
					help="Optional filter for the SVG file")

args = parser.parse_args()

for source in SOURCES:
	SRC = os.path.join('.', source)
	main(args, SRC)
