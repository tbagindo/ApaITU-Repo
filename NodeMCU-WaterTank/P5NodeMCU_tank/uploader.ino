void handleFileUpload()
{
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    if(!filename.startsWith("/")){
      filename = "/"+filename;
    }
    fsUploadFile = SPIFFS.open(filename, "w");
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile){
      fsUploadFile.write(upload.buf, upload.currentSize);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile){
      fsUploadFile.close();
    }
  }
}

void handleFileList()
{
  String path = "/";
  // Assuming there are no subdirectories
  Dir dir = SPIFFS.openDir(path);
  String output = "[";
  while(dir.next())
  {
    File entry = dir.openFile("r");
    // Separate by comma if there are multiple files
    if(output != "[")
      output += ",";
    output += String(entry.name()).substring(1);
    entry.close();
  }
  output += "]";
  server.send(200, "text/plain", output);
}
