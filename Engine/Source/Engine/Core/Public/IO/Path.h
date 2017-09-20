/*
MicroBuild
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "Core/Public/Types/Array.h"
#include "Core/Public/Types/String.h"
#include "Core/Public/Time/Time.h"

namespace Ludo {

/**
 * \brief Represents a path to a file or object on the local platforms file system.
 */
class Path
{
private:
	String m_raw;

protected:

	/** \brief Normalizes the path in-place. 
     *
     *  This involes removing trailing seperators, normalizing direction of seperators, 
     *  removal of relative directory references (.. & .) where possible, and various
     *  other operations.
     */
	void Normalize();

public:
	/// \brief Seperator of individual path segments.
	static char Seperator;

	Path();
	Path(const char* Value);
	Path(const String& Value);
    
	/** \brief Converts the path to a string.
     *
     *  \returns Path represented as a string.
     */
	String ToString() const;
    
	/** \brief Concatinates the relative path to this one.
     *
     *  \param Other Path to concatinate to this one.
     *
     *  \returns Combined represention of this path.
     *
     *  Output is indeterminate if path being concatinated is not
     *  relative. Concatinating absolute paths is not a valid operation.
     */
	Path operator +(const Path& Other) const;
	Path operator +(const String& Other) const;
    
	/** \brief Performs an equality comparison between this path and another.
     *
     *  \param Other Path to compare this one against.
     *
     *  \returns True if paths point to the same file.
     */
	bool operator ==(const Path& Other) const;
	bool operator !=(const Path& Other) const;
 
	/** \brief Gets the base name for the path, which is the filename without 
	 *         an extension.
     *
     *  \returns Base name of path.
     */
	String GetBaseName() const;

    /** \brief Gets the extension of the file this path points to.
     *
     *  \returns Extension of path.
     */
	String GetExtension() const;
    
    /** \brief Gets the filename section of this path.
     *
     *  \returns Filename of path.
     */
	String GetFilename() const;
    
    /** \brief Gets the mount of this path (drive name etc).
     *
     *  \returns Mount of path.
     */
	String GetMount() const;
    
    /** \brief Gets the directory this path is contained in.
     *
     *  \returns Directory of path.
     */
	Path GetDirectory() const;
    
    /** \brief Returns a path with the same value but a different extension.
     *
     *  \param Value Extension to change to.
     *
     *  \returns Path with changed extension.
     */
	Path ChangeExtension(const String& Value) const;
    
    /** \brief Returns a path with the same value but a different basename.
     *
     *  \param Value Base name to change to.
     *
     *  \returns Path with changed base name.
     */
	Path ChangeBaseName(const String& Value) const;
    
    /** \brief Returns a path with the same value but a different filename.
     *
     *  \param Value File name to change to.
     *
     *  \returns Path with changed file name.
     */
	Path ChangeFilename(const String& Value) const;
    
    /** \brief Returns a path with the same value but a different mount.
     *
     *  \param Value Mount to change to.
     *
     *  \returns Path with changed mount.
     */
	Path ChangeMount(const String& Value) const;
    
    /** \brief Returns a path with the same value but a different directory.
     *
     *  \param Value Directory to change to.
     *
     *  \returns Path with changed directory.
     */
	Path ChangeDirectory(const Path& Value) const;
    
    /** \brief Appends a fragment to the path and returns it.
     *
     *  \param Value            Fragment to append.
     *  \param bAddDeliminator  If true a directory seperator will be added before the fragment.
     *
     *  \returns Path with fragment appended.
     *
     *  This is a faster primary a faster way of doing concatanation, as more assumptions
     *  can be made and unneccesary work avoided. This does not perform normalization.
     */
	Path AppendFragment(const String& Value, bool bAddDeliminator = false) const;
    
    /** \brief Returns a list of files in the directory 
	 *         this path points to.
     *
     *  \returns List of files in the directory pointed to by this path.
     */
	Array<Path> GetFiles() const;
    
    /** \brief Returns a list of directories in the directory 
	 *         this path points to.
     *
     *  \returns List of directories in the directory pointed to by this path.
     */
	Array<Path> GetDirectories() const;
    
    /** \brief Returns a list of fragments that make up this path.
     *
     *  \returns List of fragments that make up this path.
     *
     *  A fragment is a part of the string deliminated by seperators. For example,
     *  the string MyFolder/MyOtherFolder/MyFile.txt is made up of the following fragments:
     *
     *  MyFolder, MyOtherFolder, MyFile.txt
     */
	Array<String> GetFragments() const;
    
    /** \brief Returns true if this path is empty.
     *
     *  \returns True if this path is empty.
     */
	bool IsEmpty() const;
    
    /** \brief Returns true if this path points to a file.
     *
     *  \returns True if this path is a file.
     */
	bool IsFile() const;

    /** \brief Returns true if this path points to a directory.
     *
     *  \returns True if this path is a directory.
     */
	bool IsDirectory() const;
    
    /** \brief Returns true if this path is relative.
     *
     *  \returns True if this path is relative.
     */
	bool IsRelative() const;
    
    /** \brief Returns true if this path is absolute.
     *
     *  \returns True if this path is absolute.
     */
	bool IsAbsolute() const;
    
    /** \brief Returns true if this path points to a root mount.
     *
     *  \returns True if this path points to a root mount.
     *
     *  A root mount on windows would be a file with a drive letter and colon, eg (C:, A:), on
     *  unix platforms this will just be a single forward slash /.
     */
	bool IsRoot() const;
	
	/** \brief Attempts to create this path as a directory. 
     *
     *  \returns True if creation was successful.
     *
     *  If the parent of this path also does not exist it then the entire
     *  structure will be created recursively.
     */
	bool CreateAsDirectory() const;
    
	/** \brief Attempts to create this path as a file. 
     *
     *  \returns True if creation was successful.
     *
     *  If the parent of this path also does not exist it then the entire
     *  structure will be created recursively.
     */
	bool CreateAsFile() const;

    /** \brief Returns true if this path exists.
     *
     *  \returns True if this path exists.
     */
	bool Exists() const;
    
    /** \brief Copies the file or directory this path points to, to another 
	 *         directory or file.
     *
     *  \param Destination Path to copy file or directory to. If a file this should be a file
     *                     path, if a directory it should be a directory.
     *
     *  \returns True on success.
     */ 
	bool Copy(const Path& Destination) const;
    
    /** \brief Deletes this path recursively.
     *
     *  \returns True on success.
     */
	bool Delete() const;
    
    /** \brief Gets the time this path was last modified.
     *
     *  \returns Time this path was last modified.
     */
	Time GetModifiedTime() const;
    
    /** \brief Creates a path that represents a relative reference from this path
	 *         to the given destination path.
     *
     *  \param Destination Destination path to make relative reference to.
     *
     *  \returns Relative reference from this path to the given destination.
     * 
     * Example: 
     *  If this path is:
     *    C:\MyFolder\Folder\File.txt
     *  And the destination is:
     *    C:\MyFolder2\Folder\File.txt
     *  The result will be:
     *    ..\..\MyFolder2\Folder\File.txt
     *
     * If either this path or the destination is relative then the desination
     * path will be returned unchanged. As there is no root to determine relative
     * path from.
     *
     * If this path or the destination do not share a mount point (eg. Different drive on windows),
     * then the destination path will be returned unchanged.
     */
	Path RelativeTo(const Path& Destination) const;
    
    /** \brief Attempts to find all file or directories that match the given filter.
     *
     *  \param Path Filter path containing wildcards to match. Path must be absolute.
     *
     *  \returns All files or directories that matched the filter.
	 * 
	 *  Filters can use * for wildcards, and ** for recursive wildcards.
	 *  eg. 
	 *	 ./MyFolder/*.ini 
	 *	 ./MyFolder/** /Project.ini
	 * 	 ./**.ini
     */
	static Array<Path> MatchFilter(const Path& path);

    /** \brief Performs the same matching logic as MatchFilter to see if this
	 *         path would match the given filter.
     *
     *  \param Other   Filter path containing wildcards to match. Path must be absolute.
     *  \param Matched If set the path contains the path fragments that the wildcards matched.
     *
     *  \returns True if this path matches the filter.
     */
	bool Matches(const Path& Other, Path* Matched = nullptr);
    
    /** \brief Gets the common path that a list of paths share, returns false if
     *         all the paths do not share a common path.
     *
     *  \param Paths  Paths to get common paths from.
     *  \param Result Common path shared between Paths list.
     *
     *  \returns True if the paths share a valid common path.
     */ 
	static bool GetCommonPath(Array<Path>& Paths, Path& Result);
    
    /** \brief Gets the current working directory
     *
     *  \returns Working directory set on the platform.
     *
     *  The working directory should never be modified or used in principle,
     *  the engine should be primarily agnostic. This is only here for 
     *  managing input from the platform.
     */
	static Path GetWorkingDirectory();
    
    /** \brief Sets the current working directory. 
     *
     *  \param Other New working directory to set.
     *
     *  The working directory should never be modified or used in principle,
     *  the engine should be primarily agnostic. This is only here for 
     *  managing input from the platform.
     */
	static void SetWorkingDirectory(const Path& Other);
    
    /** \brief Returns the part of the path following the common path that is
	 *         has been obtained from GetCommonPath.
     *
     *  \param CommonPath Common path to strip when getting the uncommon path.
     *
     *  \returns The uncommon path. If common path is not found, this path is 
     *           returned unmodified.
     */ 
	Path GetUncommonPath(Path& commonPath);

};

}; // namespace Ludo