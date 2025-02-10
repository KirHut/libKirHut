/***********************************************************************************************************************
** The KirHut Application Development Library
** kh/fstream.hpp
** Copyright © KirHut Software Company
**
** This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later
** version.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
** warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
** details.
**
** You should have received a copy of the GNU General Public License along with this program.  If not, see
** <http://www.gnu.org/licenses/>.
***********************************************************************************************************************/
#pragma once

/*!
 * \file kh/fstream.hpp Header containing object for File input and output.
 *
 * This file contains primarily three objects, the FileOutput, the FileReadStream, and the FileStream. The primary
 * difference between the three objects is that they are different types that have the three primary ways that a file
 * can be opened (for read only, for writing, and for read/write). The types are deliberately separated, but utilities
 * should use the Input and Output (or SeekableInput and SeekableOutput) classes for generic interaction with streams.
 */

#include "kh/base.hpp"
#include "kh/input.hpp"
#include "kh/output.hpp"
#include "kh/filesystem.hpp"

namespace KirHut::IO
{

// Place forward declarations here:
class FileOutput;
class FileReader;
class FileStream;

//! \cond
namespace Priv
{

typedef std::bitset<8> FileFactoryFlags;

template <IsOneOf<FileOutput, FileReader, FileStream> OpenType>
MaybeInv<OpenType> openFileFactoryImpl(FS::path const &filepath, FileFactoryFlags flags) noexcept;

} // namespace Priv
//! \endcond

/*!
 * Object representing an output stream only to a file handle.
 *
 * \headerfile kh/fstream.hpp
 *
 * All file streams are seekable as it is possible to navigate in the file stream. When this stream outputs bytes, they
 * are output **directly behind** the cursor, so tellOut() will always increase by the amount returned by
 * printsv(string_view). This object mostly wraps around a std::ofstream, so this will not have better performance
 * attributes than a standard stream, but it comes with a much easier interface to use.
 */
class KH_EXPORT FileOutput final : public SeekableOutput
{
    struct Impl;
    UPtr<Impl> im;

public:
    /*!
     * Standard destructor for FileOutput.
     *
     * This method cannot be marked as default because this class uses the Pimpl idiom, so there is no class object to
     * call a destructor on yet.
     */
    ~FileOutput() noexcept;

    /*!
     * FileOutput does not support creating copies.
     */
    FileOutput(FileOutput const &) = delete;

    /*!
     * Default move constructor.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileOutput object to move from.
     */
    FileOutput(FileOutput &&file) noexcept = default;

    /*!
     * FileOutput does not support creating copies.
     */
    FileOutput &operator=(FileOutput const &) = delete;

    /*!
     * Default move assignment operator.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileOutput object to move from.
     * \return A reference to the FileOutput that was assigned to.
     */
    FileOutput &operator=(FileOutput &&file) noexcept = default;

    /*!
     * Basic override of the Output::printsv() method that outputs to the underlying file.
     *
     * This method implements the expected Output::printsv(string_view) functionality by outputting the passed in
     * string_view to the underlying file stream, generally by placing it in a buffer prior to writing. This object is
     * implemented using a standard file output stream underneath, so this works exactly like the standard file output
     * streams.
     *
     * \param str The string_view to output to the underlying stream.
     * \throws BadState If this object has already been moved from and there is no valid underlying implementation.
     * \return The number of bytes that were actually written to the underlying stream.
     */
    size_t printsv(string_view str) override;

    /*!
     * Basic override of the Output::failedOut() method to tell if there was an underlying failure.
     *
     * This method returns the results of the std::basic_ios::bad() method as fail() should never occur because no
     * formatted output operations are ever performed on the underlying stream. Instead all formatting is done by
     * std::format.
     *
     * \return Whether or not the underlying stream has failed.
     */
    [[nodiscard]] bool failedOut() const noexcept override;

    /*!
     * \brief tellOut
     * \return
     */
    [[nodiscard]] SeekPos tellOut() const noexcept override;

    /*!
     * \brief seekOut
     * \param offset
     * \param from
     * \throws BadState If this object has already been moved from and there is no valid underlying implementation.
     */
    void seekOut(i64 offset, Relation from = CURRENT) override;

private:
    explicit FileOutput(UPtr<Impl> &&im) noexcept;
    friend MaybeInv<FileOutput> Priv::openFileFactoryImpl<FileOutput>(FS::path const &,
                                                                      Priv::FileFactoryFlags) noexcept;
};

/*!
 * \brief openFileForOutput
 * \param filePath
 * \return
 */
MaybeInv<FileOutput> openFileForOutput(FS::path filePath) noexcept;

/*!
 * Object representing an input stream only from a file handle.
 *
 * \header kh/fstream.hpp
 *
 * All file streams are seekable as it is possible to navigate in the file stream. When this reader is used for input,
 * the returned bytes will start from the current cursor position, and the cursor will advance an equal number of bytes
 * as are read from the underlying input source.
 */
class KH_EXPORT FileReader final : public SeekableInput
{
    struct Impl;
    UPtr<Impl> im;

public:
    /*!
     * Standard destructor for FileReader.
     *
     * This method cannot be marked as default because this class uses the Pimpl idiom, so there is no class object to
     * call a destructor on yet.
     */
    ~FileReader() noexcept;

    /*!
     * FileReader does not support creating copies.
     */
    FileReader(FileReader const &) = delete;

    /*!
     * Default move constructor.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileReader object to move from.
     */
    FileReader(FileReader &&file) noexcept = default;

    /*!
     * FileReader does not support creating copies.
     */
    FileReader &operator=(FileReader const &) = delete;

    /*!
     * Default move assignment operator.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileReader object to move from.
     * \return A reference to the FileReader that was assigned to.
     */
    FileReader &operator=(FileReader &&file) noexcept = default;

    /*!
     * Checks if there is additional content in the file to read, either in the buffer or the underlying data file.
     *
     * This first checks if there is any data in the file buffer, and if there is not, it then checks if there is any
     * additional data in the file itself. If there is in either case, this returns true, otherwise it returns false.
     *
     * \return If the file contains more content after the current read cursor position.
     */
    [[nodiscard]] bool moreContent() const noexcept override;

    /*!
     * Checks if the internal input stream has failed for one reason or another.
     *
     * Generally speaking, this object avoids throwing exceptions from read functions, other than std::bad_alloc or
     * BadState. Any other problem should be reported as an unrecoverable failure without an exception being propagated
     * on the stack. This method allows a user to check if this is the case.
     *
     * All input operations will return empty buffers if this method returns true.
     *
     * \return If the input stream has failed for some reason. It also returns true when methods would throw BadState.
     */
    [[nodiscard]] bool failedIn() const noexcept override;

    /*!
     * \brief readUntil
     * \param to
     * \param include
     * \return
     */
    [[nodiscard]] string readUntil(TestObj const &to, bool include = true) override;

    /*!
     * \brief fillContent
     * \param buffer
     * \param amount
     * \return
     */
    size_t fillContent(byte *buffer, size_t amount) override;

    /*!
     * \brief tellIn
     * \return
     */
    [[nodiscard]] SeekPos tellIn() const noexcept override;

    /*!
     * \brief seekIn
     * \param off
     * \param from
     */
    void seekIn(i64 off, Relation from = CURRENT) override;

    /*!
     * \brief bytesReady
     * \return
     */
    [[nodiscard]] size_t bytesReady() const noexcept override;

    /*!
     * \brief bytesRemaining
     * \return
     */
    [[nodiscard]] size_t bytesRemaining() const noexcept override;

    /*!
     * \brief readLine
     * \param upTo
     * \return
     */
    [[nodiscard]] string readLine(char upTo = '\n') override;

    /*!
     * \brief readLine
     * \param upTo
     * \return
     */
    [[nodiscard]] std::vector<byte> readLine(byte upTo) override;

    /*!
     * \brief readData
     * \param upTo
     * \param include
     * \return
     */
    [[nodiscard]] string readData(char upTo = '\n', bool include = true) override;

    /*!
     * \brief readData
     * \param upTo
     * \param include
     * \return
     */
    [[nodiscard]] std::vector<byte> readData(byte upTo, bool include = true) override;

    /*!
     * \brief fillContent
     * \param buffer
     * \param amount
     * \return
     */
    size_t fillContent(char *buffer, size_t amount) override;

private:
    explicit FileReader(UPtr<Impl> &&im) noexcept;
    friend MaybeInv<FileReader> Priv::openFileFactoryImpl<FileReader>(FS::path const &,
                                                                      Priv::FileFactoryFlags) noexcept;
};

/*!
 * \brief openFileForInput
 * \param filePath
 * \return
 */
MaybeInv<FileReader> openFileForInput(FS::path filePath) noexcept;

/*!
 * \brief The FileStream class
 */
class KH_EXPORT FileStream final : public SeekableInput, public SeekableOutput
{
    struct Impl;
    UPtr<Impl> im;

public:
    /*!
     * Standard destructor for FileStream.
     *
     * This method cannot be marked as default because this class uses the Pimpl idiom, so there is no class object to
     * call a destructor on yet.
     */
    ~FileStream();

    /*!
     * FileStream does not support creating copies.
     */
    FileStream(FileStream const &) = delete;

    /*!
     * Default move constructor.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileStream object to move from.
     */
    FileStream(FileStream &&file) noexcept = default;

    /*!
     * FileStream does not support creating copies.
     */
    FileStream &operator=(FileStream const &) = delete;

    /*!
     * Default move assignment operator.
     *
     * This just moves the underlying implementation wrapped in an unique pointer. The passed in \p file is invalidated
     * after this call and cannot be safely used at all, but can be move assigned to itself.
     *
     * \param file An rvalue reference to a FileStream object to move from.
     * \return A reference to the FileStream that was assigned to.
     */
    FileStream &operator=(FileStream &&file) noexcept = default;

    /*!
     * \brief printsv
     * \param str
     * \return
     */
    size_t printsv(string_view str) override;

    /*!
     * \brief failedOut
     * \return
     */
    [[nodiscard]] bool failedOut() const noexcept override;

    /*!
     * \brief tellOut
     * \return
     */
    [[nodiscard]] SeekPos tellOut() const noexcept override;

    /*!
     * \brief seekOut
     * \param offset
     * \param from
     */
    void seekOut(i64 offset, Relation from) override;

    /*!
     * \brief moreContent
     * \return
     */
    [[nodiscard]] bool moreContent() const noexcept override;

    /*!
     * \brief failedIn
     * \return
     */
    [[nodiscard]] bool failedIn() const noexcept override;

    /*!
     * \brief readUntil
     * \param to
     * \param include
     * \return
     */
    [[nodiscard]] string readUntil(TestObj const &to, bool include = true) override;

    /*!
     * \brief fillContent
     * \param buffer
     * \param amount
     * \return
     */
    size_t fillContent(byte *buffer, size_t amount) override;

    /*!
     * \brief tellIn
     * \return
     */
    [[nodiscard]] SeekPos tellIn() const noexcept override;

    /*!
     * \brief seekIn
     * \param off
     * \param from
     */
    void seekIn(i64 off, Relation from = CURRENT) override;

private:
    explicit FileStream(UPtr<Impl> &&im) noexcept;
    friend MaybeInv<FileStream> Priv::openFileFactoryImpl<FileStream>(FS::path const &,
                                                                      Priv::FileFactoryFlags) noexcept;
};

/*!
 * \brief openFileForIO
 * \param filePath
 * \param mustExist
 * \return
 */
MaybeInv<FileStream> openFileForIO(FS::path filePath, bool mustExist = false) noexcept;

} // namespace KirHut::IO
