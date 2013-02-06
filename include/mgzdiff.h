#ifndef __MGZ_DIFF__mgzdiff__
#define __MGZ_DIFF__mgzdiff__

#include <iostream>
/*!
 * \file mgz-diff/mgzdiff.h
 * \brief Create and manipulate binary diffs, 
 * \brief consuming predictible memory < 3 x BUFFER_SIZE,
 * \brief regardles size of diffed files.
 * \brief . 
 * \brief 
 * \author Grégoire Lejeune / Mathias Franck
 * \version 0.2
 * \date feb 2013
 */
#include <stdio.h>
#include <memory>
#include <vector>
#include <assert.h>
#include "io/file.h"

/*! \namespace mgz
 *
 * Root namespace for all merguez-IT functions and classes
 */
namespace mgz {
  
  #define BUFFER_SIZE (1UL << 25)  // 32 Mo
  #define MAGIC_NUM_DIFF 0x305A474D // 'MGZ0'
  
  #pragma pack(push,4)
  struct mgzdiff_header { // Stuff for integrity checks at decode-time.
    uint32_t magic_num;
    uint32_t crcSource;
    uint32_t crcTarget;
  };
  #pragma pack(pop)
  
  /*!
   * \class mgzdiff
   * \brief Build binary differentials between a 'source' and 'target' files, and rebuild 
   * \brief the 'target' file given the source one and the binary differential file.
   * \brief Memory consumption is predictible, and may be tuned, at the price of 'delta' files compacity:
   * \brief - When encoding, memory consumed is 4 to 5 times the size of constructor parameter 'max_buf_size'
   * \brief - When decoding, memory consumed is 2 to 3 times the size of the 'max_buf_size' that was given when encoding.
   * \param file : The abstract pathname source file
   * Either when encoding or decoding, this file is required to be an existing non empty file.
   */  class mgzdiff {
  public:
   /*! Unique constructor.
    * \brief Build an instance of mgzdiff.
    * \param file : The maximum size of the input buffers. 
    */
    mgzdiff(size_t max_buf_size = BUFFER_SIZE);
    ~mgzdiff();
    
    /*!
     * \brief Set the source
     * \param file : The abstract pathname source file
     * Either when encoding or decoding, this file is required to be an existing non empty file.
     */
    void set_source(mgz::io::file file);
    
    /*!
     * \brief Set the target
     * \param file : The abstract pathname target file.
     * When decoding, this file may not exist.
     * When encoding, it is required to be an existing non empty file.
     */
    void set_target(mgz::io::file file);
    
    /*!
     * \brief Set the delta
     * \param file : The abstract pathname delta file
     * When encoding, this file may not exist (otherwise, it will be silently overwritten).
     * When decoding, it is required to be an existing non empty file.
     */
    void set_delta(mgz::io::file file);
    
    /*!
     * \brief Create the delta from the source and target
     */
    void encode();
    
    /*!
     * \brief Create the target from the source and delta
     */
    void decode();
    
  private:
    mgzdiff(const mgzdiff& m) {assert(false);}
    void open_source(); // open-source...Mouahhhahaha ha ha ha !!!
    void open_file_for_reading(mgz::io::file filename);
    void open_file_for_writing(mgz::io::file filename);
    void close_all();
    void write_output(const char *buf, size_t buf_size, bool write_size_as_header = false);
    size_t read_input(bool read_size_in_header = false);
    size_t read_source();
    size_t adjust_buffer_sizes(const size_t source_size,const size_t target_size);
    void write_delta_header();
    mgzdiff_header read_delta_header();
    void check_decode_inputs();
    void check_encode_inputs();
  private:
    size_t max_buffer_size;
    mgz::io::file source_filename;
    mgz::io::file target_filename;
    mgz::io::file delta_filename;
    
    FILE *source_file;
    FILE *input_file;
    FILE *output_file;
    
    std::vector<char> source_buffer;
    std::vector<char> input_buffer;
    
    mgz::io::file input_filename;   // equals to target_filename (encode) or delta_filename (decode)
    mgz::io::file output_filename;  // equals to delta_filename (encode) or target_filename (decode)

  };
  
  // When sh*t happens...
  class DiffingNonExistingFileException {};
  class CantOpenFileException {};
  class CantWriteToFileException {};
  class CantReadFromFileException {};
  class DiffingFileWithItselfException {};
  class DiffingEmptyFileException {};
  class TargetOverwritesSourceException {};
  class UnexpectedEofException {} ;
  class CantCreateDeltaIndexException {};
  class CantCreateDeltaException {} ;
  class CantApplyDeltaException {} ;
  class UnknownDeltaFormatException {} ;
  class SourceHasChangedException {} ;
  class DeltaIsCorruptedException {};
}

#endif /* defined(__MGZ_DIFF__mgzdiff__) */
