#include <fstream>
#include <assert.h>

#include "diff.h"
#include "mgzdiff.h"
#include "util/exception.h"
#include "security/crc32.h"


namespace mgz {
  
  mgzdiff::~mgzdiff() {
    close_all();
  }
  
  mgzdiff::mgzdiff(size_t max_buf_size) : max_buffer_size(max_buf_size), source_file(NULL),  input_file(NULL), output_file(NULL)   {
  }

  
  void mgzdiff::set_source(mgz::io::file file) {
    source_filename = file;
  }
  
  void mgzdiff::set_target(mgz::io::file file) {
    target_filename = file;
  }
  
  void mgzdiff::set_delta(mgz::io::file file) {
    delta_filename = file;
  }
  
  void mgzdiff::encode() {
    
    if (!source_filename.exist()) {
      THROW(DiffingNonExistingFileException, "[mgzdiff/encoding] Source to diff does not exist : %s ",source_filename.get_absolute_path().c_str());
    }
    
    if (!target_filename.exist()) {
      THROW(DiffingNonExistingFileException, "[mgzdiff/encoding] Target to diff does not exist : %s ",target_filename.get_absolute_path().c_str());
    }
    
    if (target_filename.get_absolute_path()==source_filename.get_absolute_path()) {
      THROW(DiffingFileWithItselfException, "[mgzdiff/encoding] Attempting to diff the file %s with itself",target_filename.get_absolute_path().c_str());
    }
    
    size_t target_size = target_filename.size();
    size_t source_size = source_filename.size();
    
    if (target_size == 0) {
      THROW(DiffingEmptyFileException, "[mgzdiff/encoding] Attempting to diff an empty file or a dir : %s ",target_filename.get_absolute_path().c_str());
    }
    
    if (source_size == 0) {
      THROW(DiffingEmptyFileException, "[mgzdiff/encoding] Attempting to diff an empty file or a dir : %s ",source_filename.get_absolute_path().c_str());
    }
    
    int nbBuf = adjust_buffer_sizes(source_size,target_size);
    open_source();
    open_file_for_reading(target_filename);
    open_file_for_writing(delta_filename);
    
    do {
      size_t input_read = read_input();
      unsigned long source_read = read_source();
      assert(input_read && source_read);
      
      struct delta_index *index = create_delta_index(&source_buffer[0], source_read);
      if (index) {
        unsigned long delta_size = 0 ;
        char *delta = (char *)create_delta(index, &input_buffer[0],input_read, &delta_size, 0UL);
        if (delta) {
          write_output(delta, delta_size, true );
          free(delta);
        } else {
          THROW (CantCreateDeltaException, "Cannot create delta index for files %s", (source_filename.get_absolute_path() + " and "+ target_filename.get_absolute_path()).c_str());
        }
        free_delta_index(index);
      } else {
        THROW (CantCreateDeltaIndexException, "Cannot create delta index for files %s", (source_filename.get_absolute_path() + " and "+ target_filename.get_absolute_path()).c_str());
      }
      
    } while (--nbBuf);
    close_all();
  }
  
  void mgzdiff::decode() {
    if (!source_filename.exist()) {
      THROW(DiffingNonExistingFileException, "[mgzdiff/decoding] Source to update does not exist : %s ",source_filename.get_absolute_path().c_str());
    }
    
    if (!delta_filename.exist()) {
      THROW(DiffingNonExistingFileException, "[mgzdiff/decoding] Delta file does not exist : %s ",delta_filename.get_absolute_path().c_str());
    }
    
    if (target_filename.get_absolute_path()==source_filename.get_absolute_path()) {
      THROW(TargetOverwritesSourceException, "[mgzdiff/decoding] Updated file should not overwrite the source file %s",target_filename.get_absolute_path().c_str());
    }
    
    size_t delta_size = delta_filename.size();
    size_t source_size = source_filename.size();
        
    if (delta_size == 0) {
      THROW(DiffingEmptyFileException, "[mgzdiff/decoding] Attempting to apply an empty patch file (or a dir) : %s ",delta_filename.get_absolute_path().c_str());
    }
    
    if (source_size == 0) {
      THROW(DiffingEmptyFileException, "[mgzdiff/decoding] Attempting to apply a patch to an empty file (or to a dir) : %s ",source_filename.get_absolute_path().c_str());
    }
    // OK, let's do the real work !
    open_file_for_reading(delta_filename);
    open_file_for_writing(target_filename);
    open_source();

    do {
      delta_size = read_input( true );
      if (delta_size) {
        const unsigned char *data, *top;
        data = (const unsigned char *)(&input_buffer[0]);
        top = (const unsigned char *) (data + delta_size);
        unsigned long source_buffer_size;
        source_buffer_size = get_delta_hdr_size(&data, top);
        assert(source_buffer_size);
        source_buffer.resize(source_buffer_size);
        read_source();
        
        size_t write_size = 0 ;
        void * rebuilt = patch_delta(&source_buffer[0],source_buffer_size, &input_buffer[0], delta_size,&write_size);
        if (rebuilt) {
          write_output((const char *)rebuilt, write_size);
          free(rebuilt);
        } else {
          THROW (CantApplyDeltaException, "Cannot create delta index for files %s", (source_filename.get_absolute_path() + " and "+ target_filename.get_absolute_path()).c_str());
        }
      }
    } while (delta_size) ;
    close_all();
  }
  
  // -- private --
  
  void mgzdiff::close_all() {
    if(input_file) {
      fclose(input_file);
      input_file = NULL;
    }
    if(source_file) {
      fclose(source_file);
      source_file = NULL;
    }
    if(output_file) {
      fclose(output_file);
      output_file = NULL;
    }
  }

  void mgzdiff::open_source() {
    source_file = fopen(source_filename.get_path().c_str(), "rb");
    if(!source_file) {
      THROW(CantOpenFileException,"[mgzdiff]Can't open source file %s for reading", source_filename.get_path().c_str());
    }
  }

  void mgzdiff::open_file_for_reading(mgz::io::file filename) {
    input_file = fopen(filename.get_path().c_str(), "rb");
    input_filename = filename;
    if(!input_file) {
      THROW(CantOpenFileException,"[mgzdiff]Can't open target file %s for reading", filename.get_path().c_str());
    }
  }
  
  void mgzdiff::open_file_for_writing(mgz::io::file filename) {
    output_file = fopen(filename.get_path().c_str(), "wb");
    output_filename = filename;
    if(!output_file) {
      THROW(CantOpenFileException,"[mgzdiff]Can't open file %s for writing", filename.get_path().c_str());
    }
  }
  
  void mgzdiff::write_output(const char *buf, size_t buf_size, bool write_size_as_header) {
    if (buf_size >0) {
      if (write_size_as_header) {
        fwrite(&buf_size, 1, sizeof(size_t), output_file);
      }
      fwrite(buf, 1, buf_size, output_file);
      int err = ferror(output_file);
      if (err) {
        THROW(CantWriteToFileException, "[mgzdiff]Cant write to output file %s (error : %u) ", output_filename.get_path().c_str(),err);
      }
    }
  }
  
  size_t mgzdiff::read_input(bool read_size_in_header) {
    size_t bytes_read =0 ;
    size_t size_to_read = input_buffer.size();
    if (!feof(input_file)) {
      if (read_size_in_header) {
        fread(&size_to_read, 1, sizeof(size_t), input_file);
        input_buffer.resize(size_to_read);
      }
      int err = ferror(input_file);
      if (!err) {
        bytes_read = fread(&input_buffer[0], 1, size_to_read, input_file);
        err = ferror(input_file);
      }
      if (err) {
        THROW(CantReadFromFileException, "[mgzdiff]Cant read from input file %s (error : %u) ", input_filename.get_path().c_str(),err);
      }
    } else {
      THROW(UnexpectedEofException,"[mgzdiff]Attempt to read from %s after its end",input_filename.get_path().c_str());
    }
    return bytes_read;
  }

  size_t mgzdiff::read_source() {
    size_t bytes_read =0 ;
    if (!feof(source_file)) {
      bytes_read = fread(&source_buffer[0], 1, source_buffer.size(), source_file);
      int err = ferror(source_file);
      if (err) {
        THROW(CantReadFromFileException, "[mgzdiff]Cant read from source file %s (error : %u) ", source_filename.get_path().c_str(),err);
      }
    }
    return bytes_read;
  }

  /// Adjust source and target read buffer sizes, to get the same number of buf (if possible),
  /// the buffer size beeing lesser or equal to this->max_buffer_size.
  /// return 
  size_t mgzdiff::adjust_buffer_sizes(const size_t source_size,const size_t target_size) {
    int nbBuf;
    size_t sourceBufferSize,targetBufferSize;
    if ( target_size > source_size ) {
      nbBuf = (int)(target_size / max_buffer_size) + (target_size % max_buffer_size ? 1 : 0) ;
      targetBufferSize = std::min<size_t>(target_size,max_buffer_size);
      if (nbBuf * targetBufferSize - source_size < targetBufferSize) { //Difference is less than one buffer...
        sourceBufferSize=std::min<size_t>(source_size, max_buffer_size); //...then align buf size (more compact in some cases)
      } else sourceBufferSize = (int)(source_size / nbBuf) + (source_size % nbBuf ? 1 : 0) ;
    }
    else {
      nbBuf = (int)(source_size / max_buffer_size) + (source_size % max_buffer_size ? 1 : 0) ;
      sourceBufferSize = std::min<size_t>(source_size,max_buffer_size);
      if (nbBuf * sourceBufferSize - target_size < sourceBufferSize) { //Difference is less than one buffer...
        targetBufferSize=std::min<size_t>(target_size, max_buffer_size); //...then align buf size (more compact in some cases)
      } else targetBufferSize = (int)(target_size / nbBuf) + (target_size % nbBuf ? 1 : 0) ;
    }
    // TODO : Check limits, e.g: 
    //  nbBuff > sourceBufferSize ou targetBufferSize (crashes the modulo)
    //  Files too differents in size => may not map a same number of buffers.
    source_buffer.resize(sourceBufferSize);
    input_buffer.resize(targetBufferSize);
    return nbBuf;
  }
}