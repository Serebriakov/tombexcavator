package gods.sys;

import gods.base.DirectoryBase;

import java.io.*;
import java.util.TreeMap;


public class Localizer
{
	public static final String DEFAULT_LANGUAGE = "english";
	public static final String FILE_EXTENSION = ".txt";
	
	private static TreeMap<String,String> m_kv = null;
	private static String m_language = DEFAULT_LANGUAGE;
	
	public static String get_language()
	{
		return m_language;
	}

	public static String [] get_available_languages()
	{
		String [] rval = new File(get_locale_dir()).list();
		
		for (int i = 0; i < rval.length; i++)
		{
			int idx = rval[i].lastIndexOf('.');
			if (idx > 0)
			{
				rval[i] = rval[i].substring(0,idx);
			}
		}
		
		return rval;
	}
	
	private static String get_locale_dir()
	{
		return DirectoryBase.get_root() + "locale";
	}
	
	private static int [] LETTERS_TO_REPLACE = null;
	private static int [] LETTERS_REPLACED = null;
	
	public static void set_replacement_letters(int [] original, int [] replacement)
	{
		LETTERS_TO_REPLACE = original;
		LETTERS_REPLACED = replacement;
	}
	public static void set_language(String language)
	{
		m_kv = new TreeMap<String,String>();
		m_language = language;
		
		File locale_file = new File(get_locale_dir() + File.separator + language + FILE_EXTENSION);

		if (!locale_file.exists())
		{
			locale_file = new File(get_locale_dir() + File.separator + DEFAULT_LANGUAGE + FILE_EXTENSION);
		}
		
		load_any(locale_file);
	}

	public static File load(String locale_directory, String file_suffix)
	{
	  File locale_file = new File(locale_directory+File.separator+get_language()+file_suffix+FILE_EXTENSION);
	  if (!locale_file.exists())
	  {
		  locale_file = new File(locale_directory+File.separator+DEFAULT_LANGUAGE+file_suffix+FILE_EXTENSION);
	  }
	  
	  load_any(locale_file);
	  
	  return locale_file;
	}
	
	private static String replace_letters(String value)
	{
		// handling unicode here (weird locale ...)
		
		int [] buf = new int[value.length()];
		
		for (int i = 0; i < buf.length; i++)
		{
			int c = value.codePointAt(i);
			
			for (int j = 0; j < LETTERS_REPLACED.length; j++)
			{
				if (c == LETTERS_TO_REPLACE[j])
				{
					c = LETTERS_REPLACED[j];
				}
			}
			
			buf[i] = c;
		}
		
		return new String(buf,0,buf.length);
	}
	private static void load_any(File locale_file)
	{
		boolean is_unicode = false;
		
		try
		{
			BufferedReader br = new BufferedReader(new FileReader(locale_file));
			int line = 0;
			br.mark(2);
			int first_char = br.read();
			int second_char = br.read();
			is_unicode = (first_char == 254) && (second_char == 255);
			
			if (!is_unicode)
			{
				br.reset();
			}
			
			try
			{
				while (true)
				{
					line++;
					String s = br.readLine();
					String [] tokens = s.split("=");

					switch(tokens.length)
					{
					case 2:
					{
						String keyword = tokens[0].trim();
						if (is_unicode)
						{
							keyword = keyword.replaceAll("\000","");
						}
						String value = tokens[1].trim().replaceAll("\\\\n", "\n");
						if (value.length() == 0) // translation empty: same as keyword
						{
							value = keyword;
						}
						if (LETTERS_TO_REPLACE != null)
						{
							value = replace_letters(value).replaceAll("\000","");
						}
						m_kv.put(keyword,value);
						break;
					}
					case 1:
					{
						String keyword = tokens[0].trim();
						m_kv.put(keyword,keyword); // old value is replaced if found
					}
					break;
					}
				}
			}
			catch (IOException e)
			{
				br.close();
			}
		}
		catch (Exception e)
		{

		}
	}
	public static void unload(File locale_file)
	{
		if (locale_file != null)
		{
			try
			{
				BufferedReader br = new BufferedReader(new FileReader(locale_file));
				int line = 0;

				try
				{
					while (true)
					{
						line++;
						String s = br.readLine();
						String [] tokens = s.split("=");

						switch(tokens.length)
						{
						case 2:
						{
							String keyword = tokens[0].trim();
							m_kv.remove(keyword);
							break;
						}
						case 1:
						{
							String keyword = tokens[0].trim();
							m_kv.remove(keyword);
						}
						break;
						}
					}
				}
				catch (IOException e)
				{
					br.close();
				}
			}
			catch (Exception e)
			{

			}
		}
	}
	public static final String value(String key)
	{
		return value(key,false);
	}

	public static final String value(String key,boolean return_key_if_not_found)
	{
		String rval = null;
		
		if (key != null)
		{
			if (m_kv == null)
			{
				set_language(DEFAULT_LANGUAGE);
			}
			rval = m_kv.get(key);
			if (rval == null)
			{
				if (return_key_if_not_found)
				{
					rval = key;
				}
				else
				{
					rval = "["+key+"]";
				}
			}
		}
		
		return rval;
	}
}
